from glbt.lib import *

win = create_window("UwU".encode('ascii'), 640, 400)

while window_status(win) == RUNNING:
    if input_state(KEYBOARD_ESCAPE) == PRESSED:
        close_window(win)

    clear(1, 0, 1, 1)

    refresh(win)
