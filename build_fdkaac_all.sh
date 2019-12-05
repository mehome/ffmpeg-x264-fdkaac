rm -rf ./libs/libfdk-aac

for arch in arm64-v8a armeabi-v7a
do
    bash build_fdkaac.sh $arch
done
