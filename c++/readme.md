# C++ and Playdate

Should be possible - takuya on the discords solved some memory allocation
jazz usingoperator new.

Also came across https://github.com/nstbayless/playdate-cpp - recently
updated. c,f, https://devforum.play.date/t/cpp-guide-c-on-playdate/5085

----------

Unpacked the common.mk from NaOH and copied it to the project directory.

```
ln -s `pwd`/naoh-common.mk ~/Developer/PlaydateSDK/C_API/buildsupport
```

doing just that with the TNH, getting a ton of

clang: error: unsupported argument '-ahlms=build/OBJDIR' to option 'Wa,'
