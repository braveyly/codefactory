#! /bin/sh
find ./ -name "*.txt" | xargs sed -i 's/scene/profile/g'

#replace all "scene" with "profile" for all txt files in current dir
