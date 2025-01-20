# stroke_m3
Keylogger for macOS (ARM64) in C++

1. run 
```
g++ -std=c++17 -o stroke_m3 stroke_m3.cpp -framework ApplicationServices
```

2. Make sure to give accessibility rights (needed to run it)

3. Run as sudo:
```
sudo ./stroke_m3
Password:
Listening for key events... Press Ctrl+C to exit.
this is awesome/ h h h h c^C
```


