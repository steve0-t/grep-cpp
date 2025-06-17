#!/bin/bash -e

# Check if running as root
if [ "$EUID" -eq 0 ]; then
  echo "Warning: You are running this script with root privileges. Aborting"
  exit 1
fi

if [ $# -ne 1 ]; then
  echo "Usage: $0 <1-3>"
  exit 1
fi

LEVEL=$1

mkdir -p test_dirs

MAX_SUB_DIRS=0
MAX_FILES_PER_DIR=0

if [ $LEVEL -eq 1 ]; then
  MAX_SUB_DIRS=250
  MAX_FILES_PER_DIR=50
elif [ $LEVEL -eq 2 ]; then
  MAX_SUB_DIRS=500
  MAX_FILES_PER_DIR=100
elif [ $LEVEL -eq 3 ]; then
  MAX_SUB_DIRS=1000
  MAX_FILES_PER_DIR=200
else
  MAX_SUB_DIRS=100
  MAX_FILES_PER_DIR=50
fi

echo "Generating $MAX_SUB_DIRS subdirectories with $MAX_FILES_PER_DIR files each"
seq 1 $MAX_SUB_DIRS | xargs -P 4 -I {} bash -c '
  mkdir test_dirs/test_dir_{};
  for j in $(seq 1 '"$MAX_FILES_PER_DIR"'); do
    touch test_dirs/test_dir_{}/test_file_{}.txt;
    echo "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras vulputate in ante vitae condimentum. Phasellus fringilla mi sit amet mauris condimentum, sit amet vestibulum lectus aliquet. Etiam justo purus, venenatis vitae mi eu, imperdiet blandit turpis. Fusce sed lobortis mauris. Nam justo magna, euismod eget magna et, sagittis efficitur nisi. Praesent congue quis sem eget ornare. Mauris varius sagittis sem, tempus sagittis lectus sagittis a. Donec at fringilla elit. Aliquam viverra mattis risus.

Interdum et malesuada fames ac ante ipsum primis in faucibus. Donec pharetra risus et neque rutrum placerat. Maecenas eget neque libero. Etiam in dui elementum, commodo ante sed, facilisis ante. Ut auctor volutpat magna hendrerit accumsan. Morbi luctus, eros vitae feugiat vestibulum, lectus urna iaculis metus, sed sagittis metus enim at diam. Quisque quis elit quam. Suspendisse in tortor enim. Sed fermentum cursus tristique. Vivamus ipsum velit, pharetra vel placerat in, congue nec felis. Nunc congue dolor non consequat tincidunt. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia curae; Aliquam sapien ex, pulvinar fermentum enim a, auctor pharetra eros. Nunc hendrerit felis vel turpis dapibus fringilla non non libero. Praesent nulla dolor, malesuada at tellus quis, condimentum mollis urna. Donec eleifend ligula eget metus egestas pharetra.

Vestibulum ut quam ante. Pellentesque vestibulum nulla a ligula molestie convallis. Praesent sodales metus nibh. Aenean rhoncus mi vel justo sodales dapibus. Vestibulum auctor, arcu ut finibus ultricies, urna mauris sodales neque, ut ullamcorper felis justo sit amet nulla. Nam eget pretium lectus, sit amet rhoncus diam. Nulla non porttitor tellus. Aliquam in justo in dolor pharetra feugiat. Maecenas massa neque, facilisis nec suscipit non, feugiat id risus. Proin ut quam id felis facilisis mollis. Donec venenatis non tortor tincidunt posuere. Nam vehicula iaculis facilisis.

Vestibulum massa eros, lacinia sed enim suscipit, eleifend ultrices mauris. Nulla aliquet augue a dignissim vehicula. In iaculis elit leo, ac rutrum nibh ullamcorper et. Integer eu elit nec purus lobortis tempus non a diam. Fusce vulputate lectus ut lectus lacinia, malesuada sollicitudin eros interdum. Quisque facilisis et sapien id placerat. Praesent quis tortor ex. Sed vel arcu nec neque finibus laoreet eget vitae ex. Aenean auctor elementum cursus. Etiam accumsan elit tortor, ut tincidunt felis facilisis nec.

Pellentesque finibus elit et nisl faucibus tincidunt. Donec vestibulum, arcu sed mattis sodales, nibh quam viverra est, nec congue arcu libero egestas enim. Aliquam velit augue, aliquam at viverra nec, lobortis semper odio. Proin eu est ante. Donec in pulvinar ligula. Pellentesque in sem ac odio vehicula accumsan sit amet a quam. Praesent aliquet tortor lacus, sed egestas purus egestas non. In dictum pharetra nunc a porttitor. Maecenas sollicitudin dui at ex rutrum dictum. Maecenas blandit velit ante, eu porttitor nibh iaculis vitae. file {}:$j" > test_dirs/test_dir_{}/test_file_$j.txt;
  done
'
