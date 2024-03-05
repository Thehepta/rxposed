
# 在通过自己手工获取root权限的情况下使用

mount --bind tmp2 /sbin  

通过magiskpolicy 攻击修改selinux规则
./magiskpolicy  --live "allow zygote ashmem_device chr_file { open }"     
./magiskpolicy  --live "allow zygote activity_service service_manager { find }"     
./magiskpolicy  --live "allow zygote system_server binder { call transfer }"     
./magiskpolicy  --live "allow system_server zygote binder {  transfer }"     
./magiskpolicy  --live "allow zygote untrusted_app binder {  call  transfer }"     
./magiskpolicy  --live "allow zygote servicemanager binder {  call }"     