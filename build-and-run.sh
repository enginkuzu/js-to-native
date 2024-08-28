
clear
rm -f main
xxd -i lib/lib1_header.c > test-build/lib1_header.c
xxd -i lib/lib2_type.c > test-build/lib2_type.c
xxd -i lib/lib3_function.c > test-build/lib3_function.c
#clang-17 -Wall -O3 1read.c 2lex.c 3parse.c 4optimize.c 5save.c main.c js_function.c `pkg-config --cflags --libs glib-2.0` -o main
clang-17 -Wall -g -fsanitize=address 1read.c 2lex.c 3parse.c 4optimize.c 5save.c main.c js_function.c `pkg-config --cflags --libs glib-2.0` -o main

src_dir='test'
build_dir='test-build'

for filename in $src_dir/*.js; do
  filename=$(basename $filename .js)
  rm -f $build_dir/$filename'.c'
  ./main $src_dir/$filename'.js' -o $build_dir/$filename'.c'
done

for filename in $src_dir/*.js; do
  filename=$(basename $filename .js)
  rm -f $build_dir/$filename
  clang-17 -Wall -O3 $build_dir/$filename'.c' -o $build_dir/$filename
done

echo 'test    NODE DENO BUN GRAALJS QUICKJS MY_PROJECT'
for filename in $src_dir/*.js; do
  filename=$(basename $filename .js)
  printf '%-6s' "$filename"
  node=$(../4build/node-v20.11.0-linux-x64/bin/node $src_dir/$filename'.js')
  echo -n '  REF  '
  deno=$(../4build/deno-1.39.4/deno run $src_dir/$filename'.js')
  if [ "$node" == "$deno" ]; then
    echo -n 'OK   '
  else
    echo -n '!!   '
  fi
  bun=$(../4build/bun-1.0.24/bun $src_dir/$filename'.js')
  if [ "$node" == "$bun" ]; then
    echo -n 'OK  '
  else
    echo -n '!!  '
  fi
  graaljs=$(../4build/graaljs-23.1.2-linux-amd64/bin/js $src_dir/$filename'.js')
  if [ "$node" == "$graaljs" ]; then
    echo -n 'OK      '
  else
    echo -n '!!      '
  fi
  quickjs=$(../4build/quickjs/qjs $src_dir/$filename'.js')
  if [ "$node" == "$quickjs" ]; then
    echo -n 'OK      '
  else
    echo -n '!!      '
  fi
  my=$(./$build_dir/$filename)
  if [ "$node" == "$my" ]; then
    echo -n 'OK  '
  else
    echo -n '!!  '
  fi
  echo ''
done
