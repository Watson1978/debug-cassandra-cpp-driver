#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cassandra.h"

void print_error(CassFuture* future)
{
    const char* message;
    size_t message_length;
    cass_future_error_message(future, &message, &message_length);
    fprintf(stderr, "Error: %.*s\n", (int)message_length, message);
}

CassError connect_session(CassSession* session, const CassCluster* cluster)
{
    CassError rc = CASS_OK;
    CassFuture* future = cass_session_connect(session, cluster);

    cass_future_wait(future);
    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }
    cass_future_free(future);

    return rc;
}

CassError execute_query(CassSession* session, const char* query)
{
    CassError rc = CASS_OK;
    CassFuture* future = NULL;
    CassStatement* statement = cass_statement_new(query, 0);

    future = cass_session_execute(session, statement);
    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }

    cass_future_free(future);
    cass_statement_free(statement);

    return rc;
}

CassError insert_value(CassSession* session, CassStatement *statement, const char* key, const char* uuid_string)
{
    CassError rc = CASS_OK;
    CassFuture* future = NULL;
    CassUuid uuid;

    cass_statement_bind_string_by_name(statement, "key", key);
    cass_uuid_from_string(uuid_string, &uuid);
    cass_statement_bind_uuid_by_name(statement, "uuid", uuid);

    future = cass_session_execute(session, statement);

    cass_future_wait(future);

    rc = cass_future_error_code(future);
    if (rc != CASS_OK) {
        print_error(future);
    }

    cass_future_free(future);

    return rc;
}

int main()
{
    CassCluster *cluster;
    CassSession *session;
    CassError rc = CASS_OK;
    CassFuture *prepared_future;
    const CassPrepared* prepared;
    CassStatement* statement = NULL;
    const char* query = "INSERT INTO debug.sample (key, uuid) VALUES (?, ?);";

    cluster = cass_cluster_new();
    cass_cluster_set_contact_points(cluster, "127.0.0.1");
    session = cass_session_new();

    if (connect_session(session, cluster) != CASS_OK) {
        cass_cluster_free(cluster);
        cass_session_free(session);
        return -1;
    }

    execute_query(session, "CREATE KEYSPACE IF NOT EXISTS debug WITH replication = { \
                            'class': 'SimpleStrategy', 'replication_factor': '3' };");

    execute_query(session, "CREATE TABLE IF NOT EXISTS debug.sample (key text, uuid uuid, \
                                                PRIMARY KEY (key, uuid));");

    prepared_future = cass_session_prepare(session, query);
    cass_future_wait(prepared_future);
    rc = cass_future_error_code(prepared_future);
    if (rc != CASS_OK) {
        print_error(prepared_future);
        cass_cluster_free(cluster);
        cass_session_free(session);
        cass_future_free(prepared_future);
        return -1;
    }

    prepared = cass_future_get_prepared(prepared_future);
    statement = cass_prepared_bind(prepared);

    for (int i = 0; i < 1000; i++) {
        fprintf(stderr, "[%d] -------------------------\n", i);
        insert_value(session, statement, "debug", "8e53eade-55a2-4d37-8831-0ea0e0419a62");
    }

    cass_cluster_free(cluster);
    cass_session_free(session);
    cass_statement_free(statement);
    cass_prepared_free(prepared);
    cass_future_free(prepared_future);
    return 0;
}