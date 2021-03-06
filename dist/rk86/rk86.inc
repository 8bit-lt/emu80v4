﻿@NAME = "Радио-86РК"

# Тактовые частоты
@CPU_FREQUENCY = 1777777   # ВМ80, ВИ53, ВТ57
@CRT_FREQUENCY = 1333333   # ВГ75

# Основное ПЗУ и адреса перехвата
@ROM = "rk86/rk86.rom"
@F806_ADDR = 0xfb98
@F80C_ADDR = 0xfc46
@F803_ADDR = 0xfe63

# Знакогенератор
@SYM_GEN  = "rk86/sgr.bin"       

# Шрифты 8x8, 8x12, 8x16
@FONT     = "rk86/fontr.bin"

# RK-DOS
@DOS = "rk86/dos29.bin"

# Файл настроек
@OPT_FILE = "rk86/rk86.opt"

# Файл ROM-диска (если не подключен таймер)
@ROMDISK_FILE = "rk86/romdisk.bin"

# Директория SD-карты
@SD_DIR = "rk86/sdcard"



######## Window ########

EmuWindow window
window.caption = @NAME

# Default window size for window.windowStyle = fixed (default = 800, 600)
window.defaultWindowSize = 960, 640

# Window style: autosize, sizable, fixed, fullscreen (default = autosise)
window.windowStyle = autosize
#window.windowStyle = sizable

# Window scaling option: 1x, 2x, 3x, fit, bestFit, fitKeepAR (default = bestFit)
window.frameScale = 2x
#window.frameScale = fitKeepAR
#window.frameScale = bestFit

# Window fields mixing: none, mix, interlace, scanline (default = none)
#window.fieldsMixing = mix

# Image antialiasing (default = no)
window.antialiasing = no

# Aspect correction (default = no)
window.aspectCorrection = no
#window.aspectCorrection = yes

# Wide screen (16:9) (default = no)
window.wideScreen = no



######## Platform #####

Ram ram = 0x8000

Rom rom = @ROM, 0x800
Rom fdcRom = @DOS, 0x1000

Ppi8255 ppi1
Ppi8255 fdcPpi

ifndef TIMER
  Ppi8255 ppi2
endif

Dma8257 dma
dma.frequency = @CPU_FREQUENCY

Crt8275 crt
crt.frequency = @CRT_FREQUENCY
crt.dma = &dma, 2

ifdef TIMER
  Pit8253 pit
  pit.frequency = @CPU_FREQUENCY
 
  RkPit8253SoundSource sndSource
  sndSource.pit = &pit
endif

Rk86Renderer crtRenderer
crtRenderer.font = @SYM_GEN
crtRenderer.altFont = @FONT
crtRenderer.altRenderer = no
crtRenderer.visibleArea = no
crtRenderer.crt = &crt

Rk86Core core
core.window = &window
core.crtRenderer = &crtRenderer
crt.core = &core

RkFddRegister fddReg
RkFddController fdc
fdc.fddRegister = &fddReg
fdcPpi.circuit = &fdc

AddrSpace fdcAddrSpace
fdcAddrSpace.range = &fdcPpi, 0, 3
fdcAddrSpace.range = &fddReg, 4, 4
fdcAddrSpace.addrMask = 7

AddrSpace addrSpace
addrSpace.range = &ram, 0, 0x7FFF
addrSpace.range = &ppi1, 0x8000, 0x9FFF

ifdef TIMER
  addrSpace.range = &pit, 0xA000, 0xBFFF
else
  addrSpace.range = &ppi2, 0xA000, 0xBFFF
endif

addrSpace.range = &crt, 0xC000, 0xDFFF
addrSpace.writeRange = &dma, 0xE000, 0xEFFF
addrSpace.readRange = &fdcRom, 0xE000, 0xEFFF
addrSpace.range = &fdcAddrSpace, 0xF000, 0xF7FF
addrSpace.readRange = &rom, 0xF800, 0xFFFF

Cpu8080 cpu
cpu.frequency = @CPU_FREQUENCY
cpu.startAddr = 0xF800
cpu.addrSpace = &addrSpace
cpu.core = &core

dma.addrSpace = &addrSpace
dma.cpu = &cpu

RkKbdLayout kbdLayout
kbdLayout.layout = smart

RkKeyboard keyboard

RkPpi8255Circuit ppiCircuit
ppiCircuit.rkKeyboard = &keyboard

ppi1.circuit = &ppiCircuit

ifdef ROM_DISK
  RkRomDisk romDisk = @ROM_DISK_FILE
  ppi2.circuit = &romDisk
endif

ifdef SD_CARD
  RkSdController sdCard = @SD_DIR
  ppi2.circuit = &sdCard
endif



######## File I/O ########

RkFileLoader loader
loader.addrSpace = &addrSpace
loader.filter = "Файлы РК-86 (*.rkr, *.gam)|*.rkr;*.gam;*.RKR;*.GAM|Файлы RK-совместимых ПК (*.rk?)|*.rk;*.rk?;*.RK;*.RK?|Все файлы (*.*)|*"

TapeRedirector tapeInFile
tapeInFile.mode = "r"
tapeInFile.filter = "Файлы РК-86 (*.rkr, *.gam)|*.rkr;*.gam;*.RKR;*.GAM|Файлы RK-совместимых ПК (*.rk?)|*.rk;*.rk?;*.RK;*.RK?|Файлы BSM (*.bsm)|*.bsm;*.BSM|Файлы EDM (*.edm)|*.edm;*.EDM|Все файлы (*.*)|*"
#tapeInFile.fileName = "program.gam"

TapeRedirector tapeOutFile
tapeOutFile.mode = "w"
tapeOutFile.filter = "Файлы РК-86 (*.rkr)|*.rkr|Файлы RK-совместимых ПК (*.rk)|*.rk|Файлы BSM (*.bsm)|*.bsm|Файлы EDM (*.edm)|*.edm|Все файлы (*.*)|*"

FdImage diskA = 80, 2, 1, 3125
diskA.label = A
diskA.readOnly = yes
diskA.filter = "Образы дисков РК-86 (*.rkdisk, *.rdi)|*.rkdisk;*.rdi;*.RDI|Все файлы (*.*)|*"
#diskA.fileName = "rk86/RK86DOS29.rkdisk"

FdImage diskB = 80, 2, 1, 3125
diskB.label = B
diskB.filter = "Образы дисков РК-86 (*.rkdisk, *.rdi)|*.rkdisk;*.rdi;*.RDI|Все файлы (*.*)|*"

fdc.fdImage = 0, &diskA
fdc.fdImage = 1, &diskB

RkTapeInHook tapeInHook = @F806_ADDR
tapeInHook.tapeRedirector = &tapeInFile
cpu.addHook = &tapeInHook

RkTapeOutHook tapeOutHook = @F80C_ADDR
tapeOutHook.tapeRedirector = &tapeOutFile
cpu.addHook = &tapeOutHook

CloseFileHook closeFileHook = @F803_ADDR
closeFileHook.addTapeRedirector = &tapeInFile
closeFileHook.addTapeRedirector = &tapeOutFile
cpu.addHook = &closeFileHook

EmuObjectGroup tapeGrp
tapeGrp.addItem = &tapeOutHook
tapeGrp.addItem = &tapeInHook
tapeGrp.addItem = &closeFileHook
#tapeGrp.enabled = no



######## Include Saved Options ########

include @OPT_FILE



######## Configuration Window ########

ConfigTab configTab = @NAME
configTab.optFileName = @OPT_FILE

#window.frameScale
ConfigRadioSelector scaleSelector = &window, frameScale, "Масштабирование"
scaleSelector.addItem = 1x, "1x"
scaleSelector.addItem = 2x, "2x"
scaleSelector.addItem = 3x, "3x"
scaleSelector.addItem = fit, "Растянуть во все окно"
scaleSelector.addItem = fitKeepAR, "Растянуть с сохранением пропорций"
scaleSelector.addItem = bestFit, "Растянуть с целым коэффициентом"
configTab.addControl = 1, &scaleSelector

#window.windowStyle
ConfigRadioSelector styleSelector = &window, windowStyle, "Размер окна"
styleSelector.addItem = autosize, "Автоматически подстраивать"
styleSelector.addItem = sizable, "Изменяемый пользователем"
styleSelector.addItem = fixed, "Фиксированный"
configTab.addControl = 1, &styleSelector

#window.antialiasing
ConfigRadioSelector aaSelector = &window, antialiasing, "Сглаживание"
aaSelector.addItem = no, "Выкл."
aaSelector.addItem = yes, "Вкл."
configTab.addControl = 1, &aaSelector

#window.aspectCorrection
ConfigRadioSelector arSelector = &window, aspectCorrection, "Учитывать Aspect Ratio"
arSelector.addItem = no, "Нет (1:1)"
arSelector.addItem = yes, "Оригинальный AR"
configTab.addControl = 2, &arSelector

#window.wideScreen
ConfigRadioSelector wsSelector = &window, wideScreen, "Формат экрана"
wsSelector.addItem = no, "4:3"
wsSelector.addItem = yes, "16:9"
configTab.addControl = 2, &wsSelector

#crtRenderer.visibleArea
ConfigRadioSelector cropSelector = &crtRenderer, visibleArea, "Отображать только видимую область"
cropSelector.addItem = no, "Нет"
cropSelector.addItem = yes, "Да"
configTab.addControl = 2, &cropSelector

#window.fieldsMixing
ConfigRadioSelector mixSelector = &window, fieldsMixing, "Смешивание полукадров"
mixSelector.addItem = none, "Выкл."
mixSelector.addItem = mix, "Вкл."
mixSelector.addItem = interlace, "Чередование"
mixSelector.addItem = scanline, "Имитация линий растра"
configTab.addControl = 1, &mixSelector

#crtRenderer.altRenderer
ConfigRadioSelector rendSelector = &crtRenderer, altRenderer, "Шрифт ВГ8275"
rendSelector.addItem = no, "Оригинальный шрифт"
rendSelector.addItem = yes, "8×16 / 8×12 / 8×8"
configTab.addControl = 2, &rendSelector

#crtRenderer.colorMode
ConfigRadioSelector colorSelector = &crtRenderer, colorMode, "Режим цвета"
colorSelector.addItem = mono, "Ч/б"
colorSelector.addItem = color1, "Цветной (схема Толкалина)"
colorSelector.addItem = color2, "Самоцвет-М (схема Акименко)"
configTab.addControl = 2, &colorSelector

#kbdLayout.layout
ConfigRadioSelector laSelector = &kbdLayout, layout, "Раскладка клавиатуры"
laSelector.addItem = qwerty, "Qwerty"
laSelector.addItem = jcuken, "Йцукен"
laSelector.addItem = smart, "«Умная» раскладка"
configTab.addControl = 3, &laSelector

#tapeGrp.enabled
ConfigRadioSelector tapeSelector = &tapeGrp, enabled, "Перехват обращений к магнитофону"
tapeSelector.addItem = yes, "Вкл."
tapeSelector.addItem = no, "Выкл."
configTab.addControl = 3, &tapeSelector
