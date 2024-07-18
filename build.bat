lrelease translations/gui_eng.ts
lrelease translations/gui_ro.ts
mkdir build
cd build
cmake ../
cmake --build . --config Debug
cmake --build . --config Release
cmake --install . --config Release
cd ..
pause
