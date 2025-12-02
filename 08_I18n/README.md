# 08_I18n — guessnum (Autotools + gettext)

构建：
```bash
./bootstrap
./configure --prefix="$(pwd)/_inst"
make -j"$(nproc)"
make install
```

运行（英文/俄文/中文）:
```bash
_inst/bin/guessnum
LANG=ru_RU.UTF-8 _inst/bin/guessnum
LANG=zh_CN.UTF-8 _inst/bin/guessnum
```

驱动输入样例（猜 73）:
```bash
{ echo; printf "Yes\nNo\nYes\nYes\nYes\nNo\nNo\n"; } | _inst/bin/guessnum
{ echo; printf "Да\nНет\nДа\nДа\nДа\nНет\nНет\n"; } | LANG=ru_RU.UTF-8 _inst/bin/guessnum
{ echo; printf "是\n否\n是\n是\n是\n否\n否\n"; } | LANG=zh_CN.UTF-8 _inst/bin/guessnum
```
