# Pjsip Video Call 

Just an example of how to make a video call using pjsip pjsua2.

## Dependencies
- make 
- cmake > 3.14
- Gcc / Clang
- Some Voip server
  - [Issabel](https://www.issabel.org/)
  - [FreePbx](https://www.freepbx.org/)
  - [Asterisk](https://www.asterisk.org/)
- [Pjsip 2.15.1](https://www.pjsip.org/)

## Build Instructions 

1. Edit the env.h file

2. Cmake deps 
```bash
cmake -B build
```

3. Compiling
```bash
cmake --build build 
```

4. Executing
```bash
./build/video_call
```

### SoftPhone for testing
- [MicroSip](https://www.microsip.org/downloads)
- [Linphone](https://www.linphone.org/home)
