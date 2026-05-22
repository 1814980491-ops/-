# CardPileMatch

 Cocos2d-x 3.17 纸牌演示：

- 主牌区只有三堆牌。
- 备用牌区是一堆牌。
- 主牌堆顶牌与顶部牌点数差 1 时可移动到顶部牌。
- 点击备用牌堆顶牌可替换顶部牌。
- 回退会按历史记录反向移动，并恢复原牌堆的堆顶显示。
- 当主牌区的牌全部移走时，判定成功通关，否则当移完所有的备选区牌时，主牌区三堆牌都没有与手牌匹配的，则判定失败。注：not easy。
资源来自 `Resources/res`，牌面由 `card_general.png`、`number/*.png`、`suits/*.png` 拼成。

运行：

```powershell/cmd
cd D:\工程ss(具体为你所下载后解压的路径)
.\tools\setup_cocos2dx_3_17.ps1（安装/准备 Cocos2d-x 3.17 环境）
.\tools\build_win32.ps1（编译项目，生成新的 Windows 可执行文件。）
.\build-win32\bin\CardPileMatch\Debug\CardPileMatch.exe（直接启动游戏程序。）
```
