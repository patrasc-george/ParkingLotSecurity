@echo off

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
set "COMPONENTS_DIR=src\app"
for /d %%D in (%COMPONENTS_DIR%\*) do (
    echo export const environment = {> "%%D\environment.ts"
    echo. API_URL: "http://localhost:8080",>> "%%D\environment.ts"
    echo. POSTGRES_PASSWORD: "" >> "%%D\environment.ts"
    echo. };>> "%%D\environment.ts"
    echo environment.ts file created in %%D
)
echo All environment.ts files have been generated successfully!

call npm install --legacy-peer-deps
cd ..

pause