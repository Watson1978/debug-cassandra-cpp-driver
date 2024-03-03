# debug-cassandra-cpp-driver

## How to reproduce the issue

1. run `setup.sh`
1. run `docker-compose up -d`
1. run `make`
1. run `sample` which was built by `make`

## My Environment
- Apple M1 Max
- macOS 14.3.1
- clang 17.0.6

```
$ clang -v
Homebrew clang version 17.0.6
Target: arm64-apple-darwin23.3.0
Thread model: posix
InstalledDir: /opt/homebrew/opt/llvm/bin
```

## Issue
My Ruby binding for Cassandra C++ driver causes heap-use-after-free error in `cass_statement_bind_uuid_by_name`, sometimes...

```
=================================================================
==44155==ERROR: AddressSanitizer: heap-use-after-free on address 0x503000595030 at pc 0x7c733b1ad623 bp 0x7c72f1bfe570 sp 0x7c72f1bfe568
WRITE of size 4 at 0x503000595030 thread T34
    #0 0x7c733b1ad622 in datastax::internal::Atomic<int>::fetch_sub(int, datastax::internal::MemoryOrder) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad622) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #1 0x7c733b1ad472 in datastax::internal::RefCounted<datastax::internal::RefBuffer>::dec_ref() const (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad472) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #2 0x7c733b1ad3b4 in datastax::internal::core::Buffer::copy(datastax::internal::core::Buffer const&) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad3b4) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #3 0x7c733b1ad0d0 in datastax::internal::core::Buffer::operator=(datastax::internal::core::Buffer const&) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad0d0) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #4 0x7c733b1ab720 in datastax::internal::core::AbstractData::Element::operator=(datastax::internal::core::AbstractData::Element&&) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ab720) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #5 0x7c733b560eee in datastax::internal::core::AbstractData::set(unsigned long, CassUuid_) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x760eee) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #6 0x7c733b5616ae in CassError_ datastax::internal::core::AbstractData::set<CassUuid_>(datastax::StringRef, CassUuid_) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x7616ae) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #7 0x7c733b552ca9 in cass_statement_bind_uuid_by_name (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x752ca9) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #8 0x7c735d13455e in hash_cb /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/statement.c:135:22
    #9 0x591bec170124 in hash_foreach_iter /home/watson/prj/ruby/hash.c:1310:18
    #10 0x591bec3f9791 in st_general_foreach /home/watson/prj/ruby/st.c:1543:18
    #11 0x591bec15d4b7 in hash_foreach_call /home/watson/prj/ruby/hash.c:1440:15
    #12 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #13 0x591bec15d34f in rb_hash_foreach /home/watson/prj/ruby/hash.c:1464:9
    #14 0x7c735d133ec0 in statement_bind /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/statement.c:168:5
    #15 0x591bec548ef4 in vm_call_cfunc_with_frame_ /home/watson/prj/ruby/./vm_insnhelper.c:3525:11
    #16 0x591bec4fac3d in vm_sendish /home/watson/prj/ruby/./vm_insnhelper.c:5654:15
    #17 0x591bec4fac3d in vm_exec_core /home/watson/prj/ruby/insns.def:881:11
    #18 0x591bec4ebe3b in rb_vm_exec /home/watson/prj/ruby/vm.c:2552:22
    #19 0x591bec525fb8 in invoke_block /home/watson/prj/ruby/vm.c:1516:12
    #20 0x591bec525fb8 in invoke_iseq_block_from_c /home/watson/prj/ruby/vm.c:1586:16
    #21 0x591bec525fb8 in invoke_block_from_c_proc /home/watson/prj/ruby/vm.c:1684:16
    #22 0x591bec525fb8 in vm_invoke_proc /home/watson/prj/ruby/vm.c:1714:12
    #23 0x591bec5254f5 in rb_vm_invoke_proc /home/watson/prj/ruby/vm.c:1735:16
    #24 0x591bec2d9e91 in rb_proc_call_with_block_kw /home/watson/prj/ruby/proc.c:988:12
    #25 0x591bec2d9e91 in rb_proc_call_with_block /home/watson/prj/ruby/proc.c:996:12
    #26 0x7c735d130d81 in future_result_success_yield /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c
    #27 0x7c735d131541 in future_result_yielder_synchronize /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:139:9
    #28 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #29 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #30 0x7c735d131430 in future_result_yielder /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:168:12
    #31 0x7c735d131430 in future_result_yielder_thread /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:177:9
    #32 0x591bec47eaa0 in thread_do_start /home/watson/prj/ruby/thread.c:621:18
    #33 0x591bec47eaa0 in thread_start_func_2 /home/watson/prj/ruby/thread.c:667:18
    #34 0x591bec47dcfd in call_thread_start_func_2 /home/watson/prj/ruby/./thread_pthread.c:2234:5
    #35 0x591bec47dcfd in nt_start /home/watson/prj/ruby/./thread_pthread.c:2279:13
    #36 0x7c735f523559  (/usr/lib/libc.so.6+0x8f559) (BuildId: 0865c4b9ba13e0094e8b45b78dfc7a2971f536d2)
    #37 0x7c735f5a0a3b  (/usr/lib/libc.so.6+0x10ca3b) (BuildId: 0865c4b9ba13e0094e8b45b78dfc7a2971f536d2)

0x503000595030 is located 0 bytes inside of 24-byte region [0x503000595030,0x503000595048)
freed by thread T40 here:
    #0 0x591bec0a43a2 in __interceptor_free.part.0 asan_malloc_linux.cpp.o
    #1 0x7c733b1ad802 in datastax::internal::Memory::free(void*) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad802) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #2 0x7c733b1ad754 in datastax::internal::RefBuffer::operator delete(void*) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad754) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #3 0x7c733b1ad4d3 in datastax::internal::RefCounted<datastax::internal::RefBuffer>::dec_ref() const (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad4d3) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #4 0x7c733b1ac0bd in datastax::internal::core::Buffer::~Buffer() (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ac0bd) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #5 0x7c733b1adbf8 in datastax::internal::Allocator<datastax::internal::core::Buffer>::destroy(datastax::internal::core::Buffer*) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3adbf8) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #6 0x7c733b1adbcf in decltype(fp.destroy(fp0)) std::allocator_traits<datastax::internal::Allocator<datastax::internal::core::Buffer>>::_S_destroy<datastax::internal::Allocator<datastax::internal::core::Buffer>, datastax::internal::core::Buffer>(datastax::internal::Allocator<datastax::internal::core::Buffer>&, datastax::internal::core::Buffer*, int) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3adbcf) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #7 0x7c733b1adb9e in void std::allocator_traits<datastax::internal::Allocator<datastax::internal::core::Buffer>>::destroy<datastax::internal::core::Buffer>(datastax::internal::Allocator<datastax::internal::core::Buffer>&, datastax::internal::core::Buffer*) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3adb9e) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #8 0x7c733b1ada7e in void std::_Destroy<datastax::internal::core::Buffer*, datastax::internal::Allocator<datastax::internal::core::Buffer>>(datastax::internal::core::Buffer*, datastax::internal::core::Buffer*, datastax::internal::Allocator<datastax::internal::core::Buffer>&) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ada7e) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #9 0x7c733b530f92 in std::vector<datastax::internal::core::Buffer, datastax::internal::Allocator<datastax::internal::core::Buffer>>::_M_erase_at_end(datastax::internal::core::Buffer*) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x730f92) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #10 0x7c733b530e99 in std::vector<datastax::internal::core::Buffer, datastax::internal::Allocator<datastax::internal::core::Buffer>>::clear() (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x730e99) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #11 0x7c733b52ee5f in datastax::internal::core::SocketWriteBase::clear() (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x72ee5f) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #12 0x7c733b52b9fe in datastax::internal::core::SocketWriteBase::handle_write(uv_write_s*, int) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x72b9fe) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #13 0x7c733b528eeb in datastax::internal::core::SocketWriteBase::on_write(uv_write_s*, int) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x728eeb) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #14 0x7c735c4b6892  (/usr/lib/libuv.so.1+0x18892) (BuildId: 64c6ff671bf73c186fd02c0a15592461e7b9225e)

previously allocated by thread T34 here:
    #0 0x591bec0a53d9 in __interceptor_malloc (/home/watson/.rbenv/versions/3.4.0-debug/bin/ruby+0x22e3d9) (BuildId: bc271004b77b3b5e31d4aa41b27802365977fa48)
    #1 0x7c733b1ae026 in datastax::internal::Memory::malloc(unsigned long) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ae026) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #2 0x7c733b1adf6c in datastax::internal::RefBuffer::operator new(unsigned long, unsigned long) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3adf6c) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #3 0x7c733b1adf39 in datastax::internal::RefBuffer::create(unsigned long) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3adf39) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #4 0x7c733b1ac4a9 in datastax::internal::core::Buffer::Buffer(unsigned long) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ac4a9) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #5 0x7c733b56c189 in datastax::internal::core::encode_with_length(CassUuid_) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x76c189) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #6 0x7c733b560ebc in datastax::internal::core::AbstractData::set(unsigned long, CassUuid_) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x760ebc) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #7 0x7c733b5616ae in CassError_ datastax::internal::core::AbstractData::set<CassUuid_>(datastax::StringRef, CassUuid_) (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x7616ae) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #8 0x7c733b552ca9 in cass_statement_bind_uuid_by_name (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x752ca9) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f)
    #9 0x7c735d13455e in hash_cb /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/statement.c:135:22
    #10 0x591bec170124 in hash_foreach_iter /home/watson/prj/ruby/hash.c:1310:18
    #11 0x591bec3f9791 in st_general_foreach /home/watson/prj/ruby/st.c:1543:18
    #12 0x591bec15d4b7 in hash_foreach_call /home/watson/prj/ruby/hash.c:1440:15
    #13 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #14 0x591bec15d34f in rb_hash_foreach /home/watson/prj/ruby/hash.c:1464:9
    #15 0x7c735d133ec0 in statement_bind /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/statement.c:168:5
    #16 0x591bec548ef4 in vm_call_cfunc_with_frame_ /home/watson/prj/ruby/./vm_insnhelper.c:3525:11
    #17 0x591bec4fac3d in vm_sendish /home/watson/prj/ruby/./vm_insnhelper.c:5654:15
    #18 0x591bec4fac3d in vm_exec_core /home/watson/prj/ruby/insns.def:881:11
    #19 0x591bec4ebe3b in rb_vm_exec /home/watson/prj/ruby/vm.c:2552:22
    #20 0x591bec525fb8 in invoke_block /home/watson/prj/ruby/vm.c:1516:12
    #21 0x591bec525fb8 in invoke_iseq_block_from_c /home/watson/prj/ruby/vm.c:1586:16
    #22 0x591bec525fb8 in invoke_block_from_c_proc /home/watson/prj/ruby/vm.c:1684:16
    #23 0x591bec525fb8 in vm_invoke_proc /home/watson/prj/ruby/vm.c:1714:12
    #24 0x591bec5254f5 in rb_vm_invoke_proc /home/watson/prj/ruby/vm.c:1735:16
    #25 0x591bec2d9e91 in rb_proc_call_with_block_kw /home/watson/prj/ruby/proc.c:988:12
    #26 0x591bec2d9e91 in rb_proc_call_with_block /home/watson/prj/ruby/proc.c:996:12
    #27 0x7c735d130d81 in future_result_success_yield /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c
    #28 0x7c735d131541 in future_result_yielder_synchronize /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:139:9
    #29 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #30 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #31 0x7c735d131430 in future_result_yielder /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:168:12
    #32 0x7c735d131430 in future_result_yielder_thread /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:177:9
    #33 0x591bec47eaa0 in thread_do_start /home/watson/prj/ruby/thread.c:621:18
    #34 0x591bec47eaa0 in thread_start_func_2 /home/watson/prj/ruby/thread.c:667:18
    #35 0x591bec47dcfd in call_thread_start_func_2 /home/watson/prj/ruby/./thread_pthread.c:2234:5
    #36 0x591bec47dcfd in nt_start /home/watson/prj/ruby/./thread_pthread.c:2279:13
    #37 0x7c735f523559  (/usr/lib/libc.so.6+0x8f559) (BuildId: 0865c4b9ba13e0094e8b45b78dfc7a2971f536d2)

Thread T34 created by T0 here:
    #0 0x591bebfde098 in pthread_create (/home/watson/.rbenv/versions/3.4.0-debug/bin/ruby+0x167098) (BuildId: bc271004b77b3b5e31d4aa41b27802365977fa48)
    #1 0x591bec47d7f8 in native_thread_create0 /home/watson/prj/ruby/./thread_pthread.c:2151:11
    #2 0x591bec45faaf in native_thread_create_dedicated /home/watson/prj/ruby/./thread_pthread.c:2218:12
    #3 0x591bec45faaf in native_thread_create /home/watson/prj/ruby/./thread_pthread.c:2397:16
    #4 0x591bec45faaf in thread_create_core /home/watson/prj/ruby/thread.c:860:11
    #5 0x591bec45ec7b in rb_thread_create /home/watson/prj/ruby/thread.c:980:12
    #6 0x7c735d130f61 in future_thread_pool_prepare_thread /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:49:31
    #7 0x7c735d130f61 in future_queue_push /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:74:5
    #8 0x7c735d13097f in future_on_success_synchronize /home/watson/prj/ilios/tmp/x86_64-linux/ilios/3.4.0/../../../../ext/ilios/future.c:221:9
    #9 0x591bec0f78ed in rb_ensure /home/watson/prj/ruby/eval.c:1000:18
    #10 0x591bec548ef4 in vm_call_cfunc_with_frame_ /home/watson/prj/ruby/./vm_insnhelper.c:3525:11
    #11 0x591bec4f0daa in vm_sendish /home/watson/prj/ruby/./vm_insnhelper.c:5654:15
    #12 0x591bec4f0daa in vm_exec_core /home/watson/prj/ruby/insns.def:861:11
    #13 0x591bec4ebe3b in rb_vm_exec /home/watson/prj/ruby/vm.c:2552:22
    #14 0x591bec5656e1 in invoke_iseq_block_from_c /home/watson/prj/ruby/vm.c
    #15 0x591bec5656e1 in invoke_block_from_c_bh /home/watson/prj/ruby/vm.c:1604:20
    #16 0x591bec515d85 in rb_yield /home/watson/prj/ruby/./vm_eval.c
    #17 0x591bec5ce8b5 in rb_ary_collect /home/watson/prj/ruby/array.c:3601:30
    #18 0x591bec548ef4 in vm_call_cfunc_with_frame_ /home/watson/prj/ruby/./vm_insnhelper.c:3525:11
    #19 0x591bec4f0daa in vm_sendish /home/watson/prj/ruby/./vm_insnhelper.c:5654:15
    #20 0x591bec4f0daa in vm_exec_core /home/watson/prj/ruby/insns.def:861:11
    #21 0x591bec4ebe3b in rb_vm_exec /home/watson/prj/ruby/vm.c:2552:22
    #22 0x591bec525fb8 in invoke_block /home/watson/prj/ruby/vm.c:1516:12
    #23 0x591bec525fb8 in invoke_iseq_block_from_c /home/watson/prj/ruby/vm.c:1586:16
    #24 0x591bec525fb8 in invoke_block_from_c_proc /home/watson/prj/ruby/vm.c:1684:16
    #25 0x591bec525fb8 in vm_invoke_proc /home/watson/prj/ruby/vm.c:1714:12
    #26 0x591bec5254f5 in rb_vm_invoke_proc /home/watson/prj/ruby/vm.c:1735:16
    #27 0x591bec2d9a4d in rb_proc_call_kw /home/watson/prj/ruby/proc.c:962:12
    #28 0x591bec2d9a4d in rb_proc_call /home/watson/prj/ruby/proc.c:972:12
    #29 0x591bec0f20de in exec_end_procs_chain /home/watson/prj/ruby/./eval_jump.c:105:9
    #30 0x591bec0f20de in rb_ec_exec_end_proc /home/watson/prj/ruby/./eval_jump.c:120:9
    #31 0x591bec0f20de in rb_ec_teardown /home/watson/prj/ruby/eval.c:156:5
    #32 0x591bec0f2c50 in rb_ec_cleanup /home/watson/prj/ruby/eval.c:208:9
    #33 0x591bec0f3b37 in ruby_run_node /home/watson/prj/ruby/eval.c:320:12
    #34 0x591bec0ed0c6 in rb_main /home/watson/prj/ruby/./main.c:40:12
    #35 0x591bec0ed0c6 in main /home/watson/prj/ruby/./main.c:59:12
    #36 0x7c735f4bdccf  (/usr/lib/libc.so.6+0x29ccf) (BuildId: 0865c4b9ba13e0094e8b45b78dfc7a2971f536d2)

Thread T40 created by T39 here:
    #0 0x591bebfde098 in pthread_create (/home/watson/.rbenv/versions/3.4.0-debug/bin/ruby+0x167098) (BuildId: bc271004b77b3b5e31d4aa41b27802365977fa48)
    #1 0x7c735c4b7650 in uv_thread_create_ex (/usr/lib/libuv.so.1+0x19650) (BuildId: 64c6ff671bf73c186fd02c0a15592461e7b9225e)

Thread T39 created by T34 here:
    #0 0x591bebfde098 in pthread_create (/home/watson/.rbenv/versions/3.4.0-debug/bin/ruby+0x167098) (BuildId: bc271004b77b3b5e31d4aa41b27802365977fa48)
    #1 0x7c735c4b7650 in uv_thread_create_ex (/usr/lib/libuv.so.1+0x19650) (BuildId: 64c6ff671bf73c186fd02c0a15592461e7b9225e)

SUMMARY: AddressSanitizer: heap-use-after-free (/home/watson/prj/debug-cassandra-cpp-driver/lib/libcassandra.so.2+0x3ad622) (BuildId: 978d6df666effc41157bab7ab79fde471bd6494f) in datastax::internal::Atomic<int>::fetch_sub(int, datastax::internal::MemoryOrder)
Shadow bytes around the buggy address:
  0x503000594d80: fa fa 00 00 00 00 fa fa 00 00 00 00 fa fa fd fd
  0x503000594e00: fd fa fa fa 00 00 00 00 fa fa 00 00 00 00 fa fa
  0x503000594e80: fd fd fd fa fa fa 00 00 00 00 fa fa 00 00 00 00
  0x503000594f00: fa fa fd fd fd fa fa fa 00 00 00 00 fa fa 00 00
  0x503000594f80: 00 00 fa fa 00 00 00 fa fa fa 00 00 00 00 fa fa
=>0x503000595000: 00 00 00 00 fa fa[fd]fd fd fa fa fa 00 00 00 00
  0x503000595080: fa fa 00 00 00 00 fa fa 00 00 00 fa fa fa fa fa
  0x503000595100: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x503000595180: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x503000595200: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
  0x503000595280: fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa fa
Shadow byte legend (one shadow byte represents 8 application bytes):
  Addressable:           00
  Partially addressable: 01 02 03 04 05 06 07 
  Heap left redzone:       fa
  Freed heap region:       fd
  Stack left redzone:      f1
  Stack mid redzone:       f2
  Stack right redzone:     f3
  Stack after return:      f5
  Stack use after scope:   f8
  Global redzone:          f9
  Global init order:       f6
  Poisoned by user:        f7
  Container overflow:      fc
  Array cookie:            ac
  Intra object redzone:    bb
  ASan internal:           fe
  Left alloca redzone:     ca
  Right alloca redzone:    cb
==44155==ABORTING
```