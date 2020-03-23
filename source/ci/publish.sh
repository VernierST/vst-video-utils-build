#!/bin/bash

echo "Inside $1 publish"
echo "Deleting previous tags"
git tag -l | xargs -n 1 git push --delete origin
git tag | xargs git tag -d

# echo "Pushing changes to GitHub"
# #Making temporary folder to isolate necessary files
# git clone https://github.com/VernierST/vst-video-utils-build.git temp
# cd temp
# npm install

# git checkout -b circle-publish-branch
# git push --set-upstream origin circle-publish-branch

# rm VideoUtils.js
# cp ../build/bin/VideoUtils.js VideoUtils.js 
# rm vstvideoutils.js
# cp ../build/bin/vstvideoutils.js vstvideoutils.js
# rm vstvideoutils.wasm
# cp ../build/bin/vstvideoutils.wasm vstvideutils.wasm

# echo "Publishing" $1 
# npm run release -- $1 --ci --no-git.requireCleanWorkingDir

# git add VideoUtils.js
# git add vstvideoutils.js
# git add vstvideutils.wasm

# git commit -m Committing $1 changes to repo
# git push

# echo "Branch 'circle-publish-branch' created in the GitHub repo.  Please merge with master"

# cd ..
# rm -rf temp