cmd_Makefile := cd ..; /usr/lib64/node_modules/node-gyp/gyp/gyp_main.py -fmake --ignore-environment "--toplevel-dir=." -I/home/lee/code/nodejs/build/config.gypi -I/usr/lib64/node_modules/node-gyp/addon.gypi -I/home/lee/.node-gyp/0.12.6/common.gypi "--depth=." "-Goutput_dir=." "--generator-output=build" "-Dlibrary=shared_library" "-Dvisibility=default" "-Dnode_root_dir=/home/lee/.node-gyp/0.12.6" "-Dnode_gyp_dir=/usr/lib64/node_modules/node-gyp" "-Dnode_lib_file=node.lib" "-Dmodule_root_dir=/home/lee/code/nodejs" binding.gyp