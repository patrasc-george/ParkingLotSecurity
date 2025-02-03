cd server
mkdir build
cd build
cmake ../
cmake --build . --config Debug
cmake --build . --config Release
cd ../../

cd app
cd translations
lrelease eng.ts
lrelease ro.ts
cd ..

mkdir build
cd build
cmake ../
cmake --build . --config Debug
cmake --build . --config Release
cmake --install . --config Release
cd ../..

cd website
call npm install --legacy-peer-deps
call npm run build --prod
cd ..

pause
