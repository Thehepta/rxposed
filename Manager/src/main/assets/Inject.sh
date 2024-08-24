#!/system/bin/sh

echo `id`
mount_dir=$1
src=$2
dst=$3
echo "mount_dir: ${mount_dir}"
echo "src: ${src}"
echo "dst: ${dst}"
mkdir -p $mount_dir
mount -t tmpfs tmpfs $mount_dir
libdir=$(echo "$dst" | sed 's|/[^/]*$||')
echo "libdir: ${libdir}"
mkdir -p $libdir
cp $src $dst
chown  system:system $dst
chcon  u:object_r:system_lib_file:s0 $dst
chmod 0644 $dst
exit

