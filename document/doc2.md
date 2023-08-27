注入：

mount --bind tmp2 /sbin  

./magiskpolicy  --live "allow zygote ashmem_device chr_file { open }"     
./magiskpolicy  --live "allow zygote activity_service service_manager { find }"     
./magiskpolicy  --live "allow zygote system_server binder { call transfer }"     
./magiskpolicy  --live "allow system_server zygote binder {  transfer }"     
./magiskpolicy  --live "allow zygote untrusted_app binder {  call  transfer }"     