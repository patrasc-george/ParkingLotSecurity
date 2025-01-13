cd server
mkdir build
cd build
cmake ../
cmake --build . --config Debug
cmake --build . --config Release
cd ../../

cd app
cd translations
lrelease gui_eng.ts
lrelease gui_ro.ts
cd ..

mkdir build
cd build
cmake ../
cmake --build . --config Debug
cmake --build . --config Release
cmake --install . --config Release
cd ../..

cd website
npm install --legacy-peer-deps
npm audit fix
cd ..

pause
