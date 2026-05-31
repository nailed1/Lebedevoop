#!/usr/bin/env python3
# Шахматный GUI на tkinter. Бэкенд — C++ движок (libchess.dylib) через ctypes.
# Режимы: обычная игра (игрок/ИИ для каждой стороны), задачи на мат в 1-2 хода.

import ctypes
import os
import threading
import tkinter as tk
from tkinter import messagebox

# Загружаем разделяемую библиотеку C++ движка
_HERE = os.path.dirname(os.path.abspath(__file__))
_lib  = ctypes.CDLL(os.path.join(_HERE, "libchess.dylib"))

# Сигнатуры функций C API
_lib.chess_create.restype        = ctypes.c_void_p
_lib.chess_destroy.argtypes      = [ctypes.c_void_p]
_lib.chess_reset.argtypes        = [ctypes.c_void_p]
_lib.chess_get_board.argtypes    = [ctypes.c_void_p, ctypes.c_char_p]
_lib.chess_get_turn.argtypes     = [ctypes.c_void_p]
_lib.chess_get_turn.restype      = ctypes.c_char
_lib.chess_get_winner.argtypes   = [ctypes.c_void_p]
_lib.chess_get_winner.restype    = ctypes.c_char
_lib.chess_is_game_over.argtypes = [ctypes.c_void_p]
_lib.chess_is_game_over.restype  = ctypes.c_int
_lib.chess_is_draw.argtypes      = [ctypes.c_void_p]
_lib.chess_is_draw.restype       = ctypes.c_int
_lib.chess_is_in_check.argtypes  = [ctypes.c_void_p, ctypes.c_char]
_lib.chess_is_in_check.restype   = ctypes.c_int
_lib.chess_get_valid_moves.argtypes = [
    ctypes.c_void_p, ctypes.c_int, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int)]
_lib.chess_get_valid_moves.restype  = ctypes.c_int
_lib.chess_make_move.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char]
_lib.chess_make_move.restype    = ctypes.c_int
_lib.chess_get_best_move.argtypes = [
    ctypes.c_void_p, ctypes.c_char, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int)]
_lib.chess_get_best_move.restype  = ctypes.c_int
_lib.chess_load_position.argtypes = [ctypes.c_void_p, ctypes.c_char_p]
_lib.chess_load_position.restype  = ctypes.c_int
_lib.chess_find_mate_move.argtypes = [
    ctypes.c_void_p, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int)]
_lib.chess_find_mate_move.restype  = ctypes.c_int
_lib.chess_solve_puzzle.argtypes   = [ctypes.c_char_p, ctypes.c_char_p]
_lib.chess_solve_puzzle.restype    = ctypes.c_int


# Тонкая обёртка над C++ объектом Table
class ChessEngine:
    def __init__(self):
        self._h         = _lib.chess_create()
        self._board_buf = ctypes.create_string_buffer(65)
        self._pos_buf   = (ctypes.c_int * 56)()
        self._ai_buf    = (ctypes.c_int * 5)()

    def __del__(self):
        if self._h:
            _lib.chess_destroy(self._h)

    def reset(self):
        _lib.chess_reset(self._h)

    def load_position(self, filepath):
        return bool(_lib.chess_load_position(self._h, filepath.encode()))

    def board(self):
        _lib.chess_get_board(self._h, self._board_buf)
        raw = self._board_buf.value.decode()
        return [[raw[r * 8 + c] for c in range(8)] for r in range(8)]

    def turn(self):
        return _lib.chess_get_turn(self._h).decode()

    def is_game_over(self):
        return bool(_lib.chess_is_game_over(self._h))

    def is_draw(self):
        return bool(_lib.chess_is_draw(self._h))

    def winner(self):
        return _lib.chess_get_winner(self._h).decode()

    def is_in_check(self, color):
        return bool(_lib.chess_is_in_check(self._h, color.encode()))

    def valid_moves(self, r, c):
        n = _lib.chess_get_valid_moves(self._h, r, c, self._pos_buf)
        return [(self._pos_buf[i * 2], self._pos_buf[i * 2 + 1]) for i in range(n)]

    def make_move(self, fr, fc, tr, tc, promotion='Q'):
        return bool(_lib.chess_make_move(
            self._h, fr, fc, tr, tc, promotion.encode()))

    def get_best_move(self, color, depth=4):
        if _lib.chess_get_best_move(self._h, color.encode(), depth, self._ai_buf):
            fr, fc, tr, tc = self._ai_buf[:4]
            return fr, fc, tr, tc, chr(self._ai_buf[4])
        return None

    def find_mate_move(self, max_depth=2):
        buf = (ctypes.c_int * 5)()
        if _lib.chess_find_mate_move(self._h, max_depth, buf):
            return buf[0], buf[1], buf[2], buf[3], chr(buf[4])
        return None

    def find_king(self, color):
        sym = 'K' if color == 'W' else 'k'
        for r, row in enumerate(self.board()):
            for c, s in enumerate(row):
                if s == sym:
                    return (r, c)
        return None


# Визуальные константы
SQ       = 72
LIGHT    = "#F0D9B5"
DARK     = "#B58863"
SEL      = "#F6F669"
CAP_HL   = "#DAA520"
CHECK_HL = "#FF4444"
MOVE_DOT = "#3D3D3D"
AI_HL    = "#7EC8E3"
AI_DEPTH = 4

# Юникод-символы фигур
UNICODE = {
    'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',
    'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟',
}

# Список задач на мат
PUZZLES = [
    {
        "name":  "Задача 1 — Мат в 1 ход (загнанный король)",
        "file":  "puzzles/puzzle1.txt",
        "desc":  "Белые: Ф+Кр  |  Чёрные: Кр+3 пешки\nНайдите мат в один ход.",
    },
    {
        "name":  "Задача 2 — Мат в 1 ход (линейный мат)",
        "file":  "puzzles/puzzle2.txt",
        "desc":  "Белые: Ф+Кр  |  Чёрные: Кр+2 пешки\nФерзь наносит решающий удар.",
    },
    {
        "name":  "Задача 3 — Мат в 2 хода (из условия ДЗ)",
        "file":  "puzzles/puzzle3.txt",
        "desc":  "Белые: Ф+К+Кр  |  Чёрные: Кр+С+4 пешки\nПример из условия задания.",
    },
    {
        "name":  "Задача 4 — Мат в 1 ход (дальний удар)",
        "file":  "puzzles/puzzle4.txt",
        "desc":  "Белые: Ф+Кр  |  Чёрные: Кр+2 пешки\nФерзь бьёт через всю доску.",
    },
]

# Режимы обычной игры: (код, заголовок, описание)
MODES = [
    ('pvp', "Игрок  vs  Игрок",        "Оба игрока ходят вручную"),
    ('pw',  "Игрок (белые)  vs  ИИ",   "Вы за белых, ИИ за чёрных"),
    ('pb',  "ИИ  vs  Игрок (чёрные)",  "ИИ за белых, вы за чёрных"),
    ('ai',  "ИИ  vs  ИИ",              "Обе стороны управляются движком"),
]


def _sq_name(r, c):
    return "abcdefgh"[c] + str(r + 1)


def _read_solution(filepath):
    try:
        with open(filepath) as f:
            return f.read().strip()
    except Exception:
        return "(решение не найдено)"


# Начальный экран с выбором основного режима
class StartScreen:
    def __init__(self, root, app):
        self.root = root
        self.app  = app
        root.title("Шахматы")
        root.configure(bg="#1A1A2E")
        root.resizable(False, False)

        tk.Label(root, text="♔  Шахматы  ♚",
                 font=("Arial", 28, "bold"),
                 bg="#1A1A2E", fg="#E0D5C1",
                 pady=30).pack()

        tk.Label(root, text="Выберите режим",
                 font=("Arial", 13),
                 bg="#1A1A2E", fg="#9E9E9E").pack()

        tk.Frame(root, bg="#1A1A2E", height=20).pack()

        btn_cfg = dict(font=("Arial", 14, "bold"),
                       width=28, pady=14,
                       relief='flat', cursor='hand2', bd=0)

        tk.Button(root,
                  text="Обычная игра",
                  bg="#16213E", fg="#E0D5C1",
                  activebackground="#0F3460", activeforeground="white",
                  command=app.show_game_mode,
                  **btn_cfg).pack(padx=40, pady=8)

        tk.Button(root,
                  text="Задача (мат в 1-2 хода)",
                  bg="#16213E", fg="#E8C97A",
                  activebackground="#0F3460", activeforeground="white",
                  command=app.show_puzzle_selector,
                  **btn_cfg).pack(padx=40, pady=8)

        tk.Frame(root, bg="#1A1A2E", height=20).pack()

        tk.Label(root,
                 text="C++ движок  |  std=c++98  |  Minimax + Alpha-Beta",
                 font=("Arial", 9),
                 bg="#1A1A2E", fg="#555577").pack(pady=(0, 20))


# Экран выбора: кто управляет каждой из сторон
class GameModeScreen:
    def __init__(self, root, app):
        self.root = root
        self.app  = app
        root.title("Выбор режима")
        root.configure(bg="#1A1A2E")
        root.resizable(False, False)

        tk.Label(root, text="Кто играет?",
                 font=("Arial", 20, "bold"),
                 bg="#1A1A2E", fg="#E0D5C1",
                 pady=20).pack()

        frame = tk.Frame(root, bg="#1A1A2E")
        frame.pack(padx=30, pady=10)

        for code, label, desc in MODES:
            card = tk.Frame(frame, bg="#16213E",
                            bd=1, relief='groove', padx=16, pady=10)
            card.pack(fill='x', pady=5)

            tk.Label(card, text=label,
                     font=("Arial", 12, "bold"),
                     bg="#16213E", fg="#E0D5C1",
                     anchor='w').pack(fill='x')

            tk.Label(card, text=desc,
                     font=("Arial", 10),
                     bg="#16213E", fg="#AAAACC",
                     anchor='w').pack(fill='x')

            tk.Button(card, text="Выбрать",
                      font=("Arial", 10, "bold"),
                      bg="#0F3460", fg="white",
                      activebackground="#1A5276",
                      relief='flat', padx=10, pady=4,
                      cursor='hand2',
                      command=lambda c=code: app.start_game(c)
                      ).pack(anchor='e', pady=(6, 0))

        tk.Button(root, text="Назад",
                  font=("Arial", 11),
                  bg="#2C2C2C", fg="white",
                  relief='flat', padx=12, pady=5,
                  cursor='hand2',
                  command=app.show_start_screen
                  ).pack(pady=(10, 20))


# Экран выбора задачи из списка
class PuzzleSelector:
    def __init__(self, root, app):
        self.root = root
        self.app  = app
        root.title("Выбор задачи")
        root.configure(bg="#1A1A2E")
        root.resizable(False, False)

        tk.Label(root, text="Выберите задачу",
                 font=("Arial", 20, "bold"),
                 bg="#1A1A2E", fg="#E0D5C1",
                 pady=20).pack()

        frame = tk.Frame(root, bg="#1A1A2E")
        frame.pack(padx=30, pady=10)

        for pz in PUZZLES:
            card = tk.Frame(frame, bg="#16213E",
                            bd=1, relief='groove', padx=16, pady=12)
            card.pack(fill='x', pady=6)

            tk.Label(card, text=pz["name"],
                     font=("Arial", 12, "bold"),
                     bg="#16213E", fg="#E8C97A",
                     anchor='w').pack(fill='x')

            tk.Label(card, text=pz["desc"],
                     font=("Arial", 10),
                     bg="#16213E", fg="#AAAACC",
                     anchor='w', justify='left').pack(fill='x')

            pz_copy = dict(pz)
            tk.Button(card, text="Играть",
                      font=("Arial", 10, "bold"),
                      bg="#0F3460", fg="white",
                      activebackground="#1A5276",
                      relief='flat', padx=10, pady=4,
                      cursor='hand2',
                      command=lambda p=pz_copy: app.show_puzzle(p)
                      ).pack(anchor='e', pady=(6, 0))

        tk.Button(root, text="Назад",
                  font=("Arial", 11),
                  bg="#2C2C2C", fg="white",
                  relief='flat', padx=12, pady=5,
                  cursor='hand2',
                  command=app.show_start_screen
                  ).pack(pady=(10, 20))


# Общая логика отрисовки доски
class BoardCanvas:
    def __init__(self, parent):
        px = 8 * SQ
        self.canvas = tk.Canvas(parent, width=px, height=px,
                                highlightthickness=0, bg="#1A1A2E")
        self.canvas.pack()

    def draw(self, engine, selected=None, valid=None,
             ai_last=None, extra_highlights=None):
        cv   = self.canvas
        cv.delete("all")
        bd   = engine.board()
        turn = engine.turn()
        valid = valid or []
        extra_highlights = extra_highlights or {}

        # Клетка короля под шахом — красная
        check_sq = None
        if not engine.is_game_over() and engine.is_in_check(turn):
            check_sq = engine.find_king(turn)

        files = "abcdefgh"
        for r in range(8):
            for c in range(8):
                x, y   = c * SQ, (7 - r) * SQ
                base   = LIGHT if (r + c) % 2 == 0 else DARK
                fill   = extra_highlights.get((r, c), base)

                if selected == (r, c):
                    fill = SEL
                elif (r, c) in valid and bd[r][c] != '.':
                    fill = CAP_HL
                elif check_sq == (r, c):
                    fill = CHECK_HL
                elif ai_last and (r, c) in (ai_last[:2], ai_last[2:]):
                    fill = AI_HL

                cv.create_rectangle(x, y, x + SQ, y + SQ,
                                    fill=fill, outline="")

                # Подписи координат по краям доски
                lbl = DARK if (r + c) % 2 == 0 else LIGHT
                if c == 0:
                    cv.create_text(x + 3, y + 3, text=str(r + 1),
                                   font=("Arial", 7, "bold"),
                                   fill=lbl, anchor='nw')
                if r == 0:
                    cv.create_text(x + SQ - 3, y + SQ - 3,
                                   text=files[c],
                                   font=("Arial", 7, "bold"),
                                   fill=lbl, anchor='se')

                # Точка на пустых допустимых клетках
                if (r, c) in valid and bd[r][c] == '.':
                    cx, cy = x + SQ // 2, y + SQ // 2
                    rad = SQ // 7
                    cv.create_oval(cx - rad, cy - rad, cx + rad, cy + rad,
                                   fill=MOVE_DOT, stipple="gray50", outline="")

                # Фигура (тень + символ)
                sym = bd[r][c]
                if sym != '.':
                    uni = UNICODE[sym]
                    fg  = "white" if sym.isupper() else "#1A1A1A"
                    cv.create_text(x + SQ//2 + 1, y + SQ//2 + 2,
                                   text=uni,
                                   font=("Arial", int(SQ * 0.58)),
                                   fill="gray40")
                    cv.create_text(x + SQ//2, y + SQ//2,
                                   text=uni,
                                   font=("Arial", int(SQ * 0.58)),
                                   fill=fg)

    def bind_click(self, callback):
        self.canvas.bind("<Button-1>", callback)

    @staticmethod
    def rc(x, y):
        # Переводим пиксельные координаты в (row, col)
        return 7 - y // SQ, x // SQ


# Обычная партия. mode определяет, кто управляет каждой стороной:
#   pvp — оба игрок
#   pw  — игрок белыми, ИИ чёрными
#   pb  — ИИ белыми, игрок чёрными
#   ai  — ИИ vs ИИ
class ChessGUI:
    def __init__(self, root, app, mode='pvp'):
        self.root    = root
        self.app     = app
        self.engine  = ChessEngine()
        self.engine.reset()

        self.mode     = mode
        self.ai_white = mode in ('pb', 'ai')
        self.ai_black = mode in ('pw', 'ai')

        self.selected = None
        self.valid    = []
        self.ai_last  = None
        self.ai_busy  = False

        mode_labels = {
            'pvp': "Игрок vs Игрок",
            'pw':  "Игрок (бел.) vs ИИ",
            'pb':  "ИИ vs Игрок (чёрн.)",
            'ai':  "ИИ vs ИИ",
        }
        root.title(f"Шахматы — {mode_labels.get(mode, '')}")
        root.configure(bg="#2C2C2C")
        root.resizable(False, False)

        self.board_cv = BoardCanvas(root)
        self.board_cv.bind_click(self._on_click)

        bar = tk.Frame(root, bg="#2C2C2C")
        bar.pack(fill='x')

        tk.Button(bar, text="Меню",
                  command=app.show_start_screen,
                  font=("Arial", 10),
                  bg="#444", fg="white",
                  relief='flat', padx=8, pady=4,
                  cursor='hand2').pack(side='left', padx=4, pady=4)

        self.status_var = tk.StringVar(value="Ход белых")
        tk.Label(bar, textvariable=self.status_var,
                 font=("Arial", 12, "bold"),
                 bg="#2C2C2C", fg="white", pady=6
                 ).pack(side='left', padx=8)

        tk.Button(bar, text="Новая игра",
                  command=self._new_game,
                  font=("Arial", 10),
                  bg="#4A4A4A", fg="white",
                  relief='flat', padx=8, pady=4,
                  cursor='hand2').pack(side='right', padx=4, pady=4)

        self._draw()
        self._update_status()

        # Если ИИ ходит первым (белые — ИИ), запускаем сразу
        if self.ai_white and not self.engine.is_game_over():
            self.root.after(400, self._trigger_ai)

    def _is_human_turn(self):
        turn = self.engine.turn()
        return not (self.ai_white if turn == 'W' else self.ai_black)

    def _draw(self):
        self.board_cv.draw(self.engine,
                           selected=self.selected,
                           valid=self.valid,
                           ai_last=self.ai_last)

    def _update_status(self):
        eng = self.engine
        if self.ai_busy:
            self.status_var.set("ИИ думает...")
            return
        if eng.is_game_over():
            if eng.is_draw():
                self.status_var.set("Пат — ничья!")
            else:
                w = "Белые" if eng.winner() == 'W' else "Чёрные"
                self.status_var.set(f"Мат! {w} победили.")
        else:
            who  = "Белые" if eng.turn() == 'W' else "Чёрные"
            kind = " (ИИ)" if not self._is_human_turn() else ""
            chk  = "  Шах!" if eng.is_in_check(eng.turn()) else ""
            self.status_var.set(f"Ход {who}{kind}{chk}")

    def _on_click(self, event):
        eng = self.engine
        if eng.is_game_over() or self.ai_busy or not self._is_human_turn():
            return

        r, c = BoardCanvas.rc(event.x, event.y)
        if not (0 <= r < 8 and 0 <= c < 8):
            return

        bd   = eng.board()
        sym  = bd[r][c]
        turn = eng.turn()
        own  = (sym.isupper() and turn == 'W') or \
               (sym.islower() and sym != '.' and turn == 'B')

        if self.selected is None:
            if own:
                self.selected = (r, c)
                self.valid    = eng.valid_moves(r, c)
        else:
            if (r, c) in self.valid:
                self._execute_move(*self.selected, r, c, bd)
                return
            elif own:
                self.selected = (r, c)
                self.valid    = eng.valid_moves(r, c)
            else:
                self.selected = None
                self.valid    = []

        self._draw()
        self._update_status()

    def _execute_move(self, fr, fc, tr, tc, bd):
        eng   = self.engine
        sym   = bd[fr][fc]
        promo = 'Q'
        if sym.upper() == 'P' and (tr == 7 or tr == 0):
            promo = self._ask_promotion(eng.turn())

        eng.make_move(fr, fc, tr, tc, promo)
        self.selected = None
        self.valid    = []
        self.ai_last  = None
        self._draw()
        self._update_status()

        if eng.is_game_over():
            self._show_game_over()
            return

        # Если следующий ход принадлежит ИИ — запускаем его
        if not self._is_human_turn():
            self.root.after(200, self._trigger_ai)

    def _trigger_ai(self):
        eng = self.engine
        if eng.is_game_over() or self._is_human_turn():
            return
        self.ai_busy = True
        self._update_status()
        color = eng.turn()

        def worker():
            result = self.engine.get_best_move(color, AI_DEPTH)
            self.root.after(0, lambda: self._apply_ai_move(result))

        threading.Thread(target=worker, daemon=True).start()

    def _apply_ai_move(self, result):
        self.ai_busy = False
        eng = self.engine
        if result is None or eng.is_game_over():
            self._update_status()
            return
        fr, fc, tr, tc, promo = result
        self.ai_last = (fr, fc, tr, tc)
        eng.make_move(fr, fc, tr, tc, promo)
        self.selected = None
        self.valid    = []
        self._draw()
        self._update_status()

        if eng.is_game_over():
            self._show_game_over()
            return

        # В режиме ИИ vs ИИ продолжаем автоматически
        if not self._is_human_turn():
            self.root.after(400, self._trigger_ai)

    def _show_game_over(self):
        eng = self.engine
        if eng.is_draw():
            messagebox.showinfo("Конец игры", "Пат — ничья!")
        else:
            w = "Белые" if eng.winner() == 'W' else "Чёрные"
            messagebox.showinfo("Конец игры", f"{w} победили матом!")

    def _new_game(self):
        self.engine.reset()
        self.selected = None
        self.valid    = []
        self.ai_last  = None
        self.ai_busy  = False
        self._update_status()
        self._draw()
        if not self._is_human_turn() and not self.engine.is_game_over():
            self.root.after(400, self._trigger_ai)

    def _ask_promotion(self, color):
        win = tk.Toplevel(self.root)
        win.title("Превращение пешки")
        win.resizable(False, False)
        win.grab_set()
        win.transient(self.root)

        tk.Label(win, text="Выберите фигуру:",
                 font=("Arial", 12, "bold"), pady=8).pack()
        frame = tk.Frame(win)
        frame.pack(padx=10, pady=(0, 10))

        result = ['Q']
        names  = {'Q': 'Ферзь', 'R': 'Ладья', 'B': 'Слон', 'N': 'Конь'}

        for pt in ('Q', 'R', 'B', 'N'):
            uni = UNICODE[pt if color == 'W' else pt.lower()]

            def pick(p=pt):
                result[0] = p
                win.destroy()

            tk.Button(frame, text=f"{uni}\n{names[pt]}",
                      font=("Arial", 16), width=4, height=2,
                      command=pick, relief='groove',
                      cursor='hand2').pack(side='left', padx=4)

        self.root.update_idletasks()
        bx = self.root.winfo_rootx() + self.root.winfo_width()  // 2
        by = self.root.winfo_rooty() + self.root.winfo_height() // 2
        win.update_idletasks()
        win.geometry(
            f"+{bx - win.winfo_width()//2}+{by - win.winfo_height()//2}")
        self.root.wait_window(win)
        return result[0]


# Режим задачи: пользователь играет за чёрных, ИИ — за белых.
# Решение отображается в боковой панели.
class PuzzleGUI:
    def __init__(self, root, app, puzzle):
        self.root    = root
        self.app     = app
        self.puzzle  = puzzle
        self.engine  = ChessEngine()

        root.title(f"Задача — {puzzle['name']}")
        root.configure(bg="#1A1A2E")
        root.resizable(False, False)

        outer = tk.Frame(root, bg="#1A1A2E")
        outer.pack(fill='both', expand=True)

        left  = tk.Frame(outer, bg="#1A1A2E")
        left.pack(side='left')
        right = tk.Frame(outer, bg="#16213E", width=260, padx=16, pady=16)
        right.pack(side='left', fill='y')
        right.pack_propagate(False)

        self.board_cv = BoardCanvas(left)
        self.board_cv.bind_click(self._on_click)

        bot = tk.Frame(left, bg="#2C2C2C")
        bot.pack(fill='x')

        tk.Button(bot, text="Меню",
                  command=app.show_start_screen,
                  font=("Arial", 10), bg="#444", fg="white",
                  relief='flat', padx=8, pady=4,
                  cursor='hand2').pack(side='left', padx=4, pady=4)

        self.status_var = tk.StringVar()
        tk.Label(bot, textvariable=self.status_var,
                 font=("Arial", 11, "bold"),
                 bg="#2C2C2C", fg="white",
                 pady=6).pack(side='left', padx=8)

        tk.Button(bot, text="Сброс",
                  command=self._reset,
                  font=("Arial", 10), bg="#4A4A4A", fg="white",
                  relief='flat', padx=8, pady=4,
                  cursor='hand2').pack(side='right', padx=4, pady=4)

        # Правая панель: условие, решение, история ходов
        tk.Label(right, text="Условие задачи",
                 font=("Arial", 13, "bold"),
                 bg="#16213E", fg="#E8C97A",
                 anchor='w').pack(fill='x', pady=(0, 6))

        tk.Label(right, text=puzzle["name"],
                 font=("Arial", 10, "bold"),
                 bg="#16213E", fg="#E0D5C1",
                 anchor='w', wraplength=228, justify='left').pack(fill='x')

        tk.Label(right, text=puzzle["desc"],
                 font=("Arial", 9),
                 bg="#16213E", fg="#AAAACC",
                 anchor='w', wraplength=228, justify='left').pack(fill='x', pady=(4, 12))

        tk.Label(right, text="Вы играете: чёрные",
                 font=("Arial", 10, "bold"),
                 bg="#16213E", fg="#88CCFF", anchor='w').pack(fill='x')

        tk.Label(right, text="ИИ играет: белые",
                 font=("Arial", 10, "bold"),
                 bg="#16213E", fg="#FFAA55", anchor='w').pack(fill='x', pady=(2, 12))

        tk.Frame(right, bg="#334466", height=1).pack(fill='x', pady=6)

        tk.Label(right, text="Решение (спойлер)",
                 font=("Arial", 11, "bold"),
                 bg="#16213E", fg="#E8C97A", anchor='w').pack(fill='x', pady=(4, 4))

        self.sol_text = tk.Text(right, height=8, width=28,
                                font=("Courier", 10),
                                bg="#0D1B2A", fg="#88FF88",
                                relief='flat', state='disabled', wrap='word')
        self.sol_text.pack(fill='x')

        tk.Frame(right, bg="#334466", height=1).pack(fill='x', pady=10)

        tk.Label(right, text="История ходов",
                 font=("Arial", 11, "bold"),
                 bg="#16213E", fg="#E8C97A", anchor='w').pack(fill='x', pady=(0, 4))

        self.history_text = tk.Text(right, height=8, width=28,
                                    font=("Courier", 10),
                                    bg="#0D1B2A", fg="#DDDDDD",
                                    relief='flat', state='disabled', wrap='word')
        self.history_text.pack(fill='x')

        self.selected  = None
        self.valid     = []
        self.ai_busy   = False
        self.ai_last   = None
        self.move_num  = 1
        self.history   = []

        self._reset()

    def _reset(self):
        filepath = os.path.join(_HERE, self.puzzle["file"])
        self.engine.load_position(filepath)
        self.selected = None
        self.valid    = []
        self.ai_busy  = False
        self.ai_last  = None
        self.move_num = 1
        self.history  = []

        # Генерируем файл решения и показываем его
        sol_path = filepath.replace(".txt", "_sol.txt")
        _lib.chess_solve_puzzle(filepath.encode(), sol_path.encode())
        self._set_text(self.sol_text, _read_solution(sol_path))
        self._set_text(self.history_text, "")

        self._draw()
        self._update_status()

        if not self.engine.is_game_over():
            self.root.after(600, self._trigger_ai)

    def _set_text(self, widget, content):
        widget.config(state='normal')
        widget.delete('1.0', 'end')
        widget.insert('end', content)
        widget.config(state='disabled')

    def _append_history(self, text):
        self.history.append(text)
        self._set_text(self.history_text, "\n".join(self.history))

    def _draw(self):
        self.board_cv.draw(self.engine,
                           selected=self.selected,
                           valid=self.valid,
                           ai_last=self.ai_last)

    def _update_status(self):
        eng = self.engine
        if self.ai_busy:
            self.status_var.set("ИИ думает...")
            return
        if eng.is_game_over():
            if eng.is_draw():
                self.status_var.set("Пат — ничья!")
            else:
                w = "Белые" if eng.winner() == 'W' else "Чёрные"
                self.status_var.set(f"Мат! {w} победили.")
        else:
            who = "Белые (ИИ)" if eng.turn() == 'W' else "Ваш ход (чёрные)"
            chk = "  ШАХ!" if eng.is_in_check(eng.turn()) else ""
            self.status_var.set(who + chk)

    def _on_click(self, event):
        eng = self.engine
        if eng.is_game_over() or self.ai_busy or eng.turn() == 'W':
            return

        r, c = BoardCanvas.rc(event.x, event.y)
        if not (0 <= r < 8 and 0 <= c < 8):
            return

        bd  = eng.board()
        sym = bd[r][c]
        own = sym.islower() and sym != '.'

        if self.selected is None:
            if own:
                self.selected = (r, c)
                self.valid    = eng.valid_moves(r, c)
        else:
            if (r, c) in self.valid:
                self._execute_black_move(*self.selected, r, c, bd)
                return
            elif own:
                self.selected = (r, c)
                self.valid    = eng.valid_moves(r, c)
            else:
                self.selected = None
                self.valid    = []

        self._draw()
        self._update_status()

    def _execute_black_move(self, fr, fc, tr, tc, bd):
        eng   = self.engine
        promo = 'Q'

        move_label = f"{self.move_num}... {_sq_name(fr,fc)}-{_sq_name(tr,tc)}"
        eng.make_move(fr, fc, tr, tc, promo)
        self.selected = None
        self.valid    = []
        self.ai_last  = None
        self._append_history(move_label)
        self._draw()
        self._update_status()

        if eng.is_game_over():
            self._on_game_over()
            return

        self.move_num += 1
        self.root.after(300, self._trigger_ai)

    def _trigger_ai(self):
        eng = self.engine
        if eng.is_game_over() or eng.turn() != 'W':
            return
        self.ai_busy = True
        self._update_status()

        def worker():
            # Сначала ищем форсированный мат, иначе — обычный минимакс
            result = self.engine.find_mate_move(2)
            if result is None:
                result = self.engine.get_best_move('W', 3)
            self.root.after(0, lambda: self._apply_ai_move(result))

        threading.Thread(target=worker, daemon=True).start()

    def _apply_ai_move(self, result):
        self.ai_busy = False
        eng = self.engine
        if result is None or eng.is_game_over():
            self._update_status()
            return

        fr, fc, tr, tc, promo = result
        move_label = f"{self.move_num}. {_sq_name(fr,fc)}-{_sq_name(tr,tc)}"
        self.ai_last = (fr, fc, tr, tc)
        eng.make_move(fr, fc, tr, tc, promo)
        self.selected = None
        self.valid    = []
        self._append_history(move_label)
        self._draw()
        self._update_status()

        if eng.is_game_over():
            self._on_game_over()

    def _on_game_over(self):
        eng = self.engine
        if eng.is_draw():
            msg = "Пат — ничья!"
        elif eng.winner() == 'W':
            msg = "Мат! Белые поставили мат чёрным.\nЗадача успешно решена!"
        else:
            msg = "Мат! Чёрные победили. (Неожиданно!)"
        messagebox.showinfo("Конец задачи", msg)


# Контроллер приложения: управляет переключением экранов
class App:
    def __init__(self, root):
        self.root = root
        self.show_start_screen()

    def _clear(self):
        for w in self.root.winfo_children():
            w.destroy()

    def show_start_screen(self):
        self._clear()
        StartScreen(self.root, self)

    def show_game_mode(self):
        self._clear()
        GameModeScreen(self.root, self)

    def start_game(self, mode):
        self._clear()
        ChessGUI(self.root, self, mode)

    def show_puzzle_selector(self):
        self._clear()
        PuzzleSelector(self.root, self)

    def show_puzzle(self, puzzle):
        self._clear()
        PuzzleGUI(self.root, self, puzzle)


def main():
    root = tk.Tk()
    root.resizable(False, False)
    App(root)
    root.mainloop()


if __name__ == '__main__':
    main()
