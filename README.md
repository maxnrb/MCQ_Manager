# MCQ_Manager
Automatic reading system for mutliple choice questions

## Requirements:
-Install pistache:
```
git clone https://github.com/oktal/pistache.git
cd pistache
git submodule update --init
mkdir -p {build,prefix}
cd build
cmake -G "Unix Makefiles" \
        -DCMAKE_BUILD_TYPE=Release \
        -DPISTACHE_BUILD_EXAMPLES=false \
        -DPISTACHE_BUILD_TESTS=false \
        -DPISTACHE_BUILD_DOCS=false \
        -DPISTACHE_USE_SSL=false \
        -DCMAKE_INSTALL_PREFIX=$PWD/../prefix \
	../
make -j
make install
```

