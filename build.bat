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

powershell -NoExit -Command "$ErrorActionPreference = 'Stop'; $srcPath = '.\src\app'; Get-ChildItem -Path $srcPath -Recurse -File | Where-Object { $_.Extension -match '\.js$|\.ts$|\.html$|\.json$' } | ForEach-Object { Write-Host 'Processing file:' $_.FullName; try { (Get-Content $_.FullName -Raw) -replace '{{API_URL}}', '%API_URL%' | Set-Content $_.FullName -Force; } catch { Write-Host 'Error processing file' $_.FullName -ForegroundColor Red; } }; exit"

powershell -NoExit -Command "$ErrorActionPreference = 'Stop'; $srcPath = '.\src\app'; Get-ChildItem -Path $srcPath -Recurse -File | Where-Object { $_.Extension -match '\.js$|\.ts$|\.html$|\.json$' } | ForEach-Object { Write-Host 'Processing file:' $_.FullName; try { (Get-Content $_.FullName -Raw) -replace '{{POSTGRES_PASSWORD}}', '%POSTGRES_PASSWORD%' | Set-Content $_.FullName -Force; } catch { Write-Host 'Error processing file' $_.FullName -ForegroundColor Red; } }; exit"

call npm install --legacy-peer-deps
call npm run build --prod
cd ..

pause
