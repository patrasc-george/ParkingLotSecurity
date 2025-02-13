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
set API_URL=http://localhost:8080
set POSTGRES_PASSWORD=""

find . -type f -exec sed -i "s|{{API_URL}}|%API_URL%|g" {} \;
find . -type f -exec sed -i "s|{{POSTGRES_PASSWORD}}|%POSTGRES_PASSWORD%|g" {} \;

call npm install --legacy-peer-deps
call npm run build --prod
cd ..

pause
