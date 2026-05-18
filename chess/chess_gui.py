#!/usr/bin/env python3
"""
Chess GUI — tkinter front-end backed by the C++ chess engine (libchess.dylib).
Supports two-player mode and Player vs AI mode (minimax + alpha-beta, depth 3).
"""
import ctypes
import os
import threading
import tkinter as tk
from tkinter import messagebox

#  Load shared library ─
_HERE = os.path.dirname(os.path.abspath(__file__))
_lib  = ctypes.CDLL(os.path.join(_HERE, "libchess.dylib"))

_lib.chess_create.restype       = ctypes.c_void_p
_lib.chess_destroy.argtypes     = [ctypes.c_void_p]
_lib.chess_reset.argtypes       = [ctypes.c_void_p]
_lib.chess_get_board.argtypes   = [ctypes.c_void_p, ctypes.c_char_p]
_lib.chess_get_turn.argtypes    = [ctypes.c_void_p]
_lib.chess_get_turn.restype     = ctypes.c_char
_lib.chess_get_winner.argtypes  = [ctypes.c_void_p]
_lib.chess_get_winner.restype   = ctypes.c_char
_lib.chess_is_game_over.argtypes = [ctypes.c_void_p]
_lib.chess_is_game_over.restype  = ctypes.c_int
_lib.chess_is_draw.argtypes     = [ctypes.c_void_p]
_lib.chess_is_draw.restype      = ctypes.c_int
_lib.chess_is_in_check.argtypes = [ctypes.c_void_p, ctypes.c_char]
_lib.chess_is_in_check.restype  = ctypes.c_int
_lib.chess_get_valid_moves.argtypes = [
    ctypes.c_void_p, ctypes.c_int, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
]
_lib.chess_get_valid_moves.restype = ctypes.c_int
_lib.chess_make_move.argtypes = [
    ctypes.c_void_p,
    ctypes.c_int, ctypes.c_int, ctypes.c_int, ctypes.c_int,
    ctypes.c_char,
]
_lib.chess_make_move.restype = ctypes.c_int
_lib.chess_get_best_move.argtypes = [
    ctypes.c_void_p, ctypes.c_char, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int),
]
_lib.chess_get_best_move.restype = ctypes.c_int


#  C++ engine wrapper 
class ChessEngine:
    def __init__(self):
        self._h        = _lib.chess_create()
        self._board_buf = ctypes.create_string_buffer(65)
        self._pos_buf   = (ctypes.c_int * 56)()
        self._ai_buf    = (ctypes.c_int * 5)()

    def __del__(self):
        if self._h:
            _lib.chess_destroy(self._h)

    def reset(self):
        _lib.chess_reset(self._h)

    def board(self) -> list[list[str]]:
        _lib.chess_get_board(self._h, self._board_buf)
        raw = self._board_buf.value.decode()
        return [[raw[r * 8 + c] for c in range(8)] for r in range(8)]

    def turn(self) -> str:
        return _lib.chess_get_turn(self._h).decode()

    def is_game_over(self) -> bool:
        return bool(_lib.chess_is_game_over(self._h))

    def is_draw(self) -> bool:
        return bool(_lib.chess_is_draw(self._h))

    def winner(self) -> str:
        return _lib.chess_get_winner(self._h).decode()

    def is_in_check(self, color: str) -> bool:
        return bool(_lib.chess_is_in_check(self._h, color.encode()))

    def valid_moves(self, r: int, c: int) -> list[tuple[int, int]]:
        n = _lib.chess_get_valid_moves(self._h, r, c, self._pos_buf)
        return [(self._pos_buf[i * 2], self._pos_buf[i * 2 + 1]) for i in range(n)]

    def make_move(self, fr: int, fc: int, tr: int, tc: int, promotion: str = 'Q') -> bool:
        return bool(_lib.chess_make_move(
            self._h, fr, fc, tr, tc, promotion.encode()
        ))

    def get_best_move(self, color: str, depth: int = 5):
        """Returns (fr, fc, tr, tc, promo_char) or None."""
        if _lib.chess_get_best_move(self._h, color.encode(), depth, self._ai_buf):
            fr, fc, tr, tc = self._ai_buf[:4]
            promo = chr(self._ai_buf[4])
            return fr, fc, tr, tc, promo
        return None

    def find_king(self, color: str) -> tuple[int, int] | None:
        sym = 'K' if color == 'W' else 'k'
        for r, row in enumerate(self.board()):
            for c, s in enumerate(row):
                if s == sym:
                    return (r, c)
        return None


#  Visual constants 
SQ       = 80
LIGHT    = "#F0D9B5"
DARK     = "#B58863"
SEL      = "#F6F669"
CAP_HL   = "#DAA520"
CHECK_HL = "#FF4444"
MOVE_DOT = "#3D3D3D"
AI_HL    = "#7EC8E3"   # light blue — square the AI just moved from/to

UNICODE = {
    'K': '♔', 'Q': '♕', 'R': '♖', 'B': '♗', 'N': '♘', 'P': '♙',
    'k': '♚', 'q': '♛', 'r': '♜', 'b': '♝', 'n': '♞', 'p': '♟',
}

AI_DEPTH = 5   # half-moves; increase for stronger play (4 is noticeably slower)


#  GUI ─
class ChessGUI:
    def __init__(self, root: tk.Tk):
        self.root = root
        self.root.title("Chess  [C++ engine]")
        self.root.resizable(False, False)

        self.engine    = ChessEngine()
        self.selected: tuple[int, int] | None = None
        self.valid:    list[tuple[int, int]]   = []
        self.ai_last:  tuple[int, int, int, int] | None = None  # (fr,fc,tr,tc)

        # AI mode: player=White, AI=Black
        self.ai_mode   = False
        self.ai_busy   = False   # True while AI is thinking

        #  Layout 
        board_px = 8 * SQ
        self.canvas = tk.Canvas(root, width=board_px, height=board_px,
                                highlightthickness=0)
        self.canvas.pack()
        self.canvas.bind("<Button-1>", self._on_click)

        bar = tk.Frame(root, bg="#2C2C2C")
        bar.pack(fill='x')

        self.status_var = tk.StringVar(value="White's turn")
        tk.Label(bar, textvariable=self.status_var,
                 font=("Arial", 13, "bold"),
                 bg="#2C2C2C", fg="white", pady=6).pack(side='left', padx=12)

        self.ai_btn_var = tk.StringVar(value="AI: OFF")
        tk.Button(bar, textvariable=self.ai_btn_var,
                  command=self._toggle_ai,
                  font=("Arial", 11),
                  bg="#4A6FA5", fg="white",
                  relief='flat', padx=10, pady=4,
                  cursor='hand2').pack(side='right', padx=4, pady=4)

        tk.Button(bar, text="New Game",
                  command=self._new_game,
                  font=("Arial", 11),
                  bg="#4A4A4A", fg="white",
                  relief='flat', padx=10, pady=4,
                  cursor='hand2').pack(side='right', padx=4, pady=4)

        self._draw()
        self._update_status()

    #  Coords 
    @staticmethod
    def _xy(r: int, c: int) -> tuple[int, int]:
        return c * SQ, (7 - r) * SQ

    @staticmethod
    def _rc(x: int, y: int) -> tuple[int, int]:
        return 7 - y // SQ, x // SQ

    #  Draw 
    def _draw(self):
        cv  = self.canvas
        cv.delete("all")
        eng  = self.engine
        bd   = eng.board()
        turn = eng.turn()

        check_sq = None
        if not eng.is_game_over() and eng.is_in_check(turn):
            check_sq = eng.find_king(turn)

        files = "abcdefgh"
        for r in range(8):
            for c in range(8):
                x, y = self._xy(r, c)
                base = LIGHT if (r + c) % 2 == 0 else DARK

                # Square colour priority
                if self.selected == (r, c):
                    fill = SEL
                elif (r, c) in self.valid and bd[r][c] != '.':
                    fill = CAP_HL
                elif check_sq == (r, c):
                    fill = CHECK_HL
                elif self.ai_last and (r, c) in (self.ai_last[:2], self.ai_last[2:]):
                    fill = AI_HL
                else:
                    fill = base

                cv.create_rectangle(x, y, x + SQ, y + SQ, fill=fill, outline="")

                # Coordinate labels
                lbl = DARK if (r + c) % 2 == 0 else LIGHT
                if c == 0:
                    cv.create_text(x + 3, y + 3, text=str(r + 1),
                                   font=("Arial", 8, "bold"), fill=lbl, anchor='nw')
                if r == 0:
                    cv.create_text(x + SQ - 3, y + SQ - 3, text=files[c],
                                   font=("Arial", 8, "bold"), fill=lbl, anchor='se')

                # Valid-move dot (empty squares)
                if (r, c) in self.valid and bd[r][c] == '.':
                    cx, cy = x + SQ // 2, y + SQ // 2
                    rad = SQ // 7
                    cv.create_oval(cx - rad, cy - rad, cx + rad, cy + rad,
                                   fill=MOVE_DOT, stipple="gray50", outline="")

                # Piece
                sym = bd[r][c]
                if sym != '.':
                    uni = UNICODE[sym]
                    fg  = "white" if sym.isupper() else "#1A1A1A"
                    cv.create_text(x + SQ//2 + 1, y + SQ//2 + 2,
                                   text=uni, font=("Arial", int(SQ * 0.60)), fill="gray40")
                    cv.create_text(x + SQ//2,     y + SQ//2,
                                   text=uni, font=("Arial", int(SQ * 0.60)), fill=fg)

    def _update_status(self):
        eng = self.engine
        if self.ai_busy:
            self.status_var.set("AI is thinking...")
            return
        if eng.is_game_over():
            if eng.is_draw():
                self.status_var.set("Stalemate — it's a draw!")
            else:
                w = "White" if eng.winner() == 'W' else "Black"
                self.status_var.set(f"Checkmate! {w} wins.")
        else:
            who = "White" if eng.turn() == 'W' else "Black"
            ai_tag = " (AI)" if (self.ai_mode and eng.turn() == 'B') else ""
            suffix = "  |  Check!" if eng.is_in_check(eng.turn()) else ""
            self.status_var.set(f"{who}{ai_tag}'s turn{suffix}")

    #  Player input 
    def _on_click(self, event: tk.Event):
        eng = self.engine
        if eng.is_game_over() or self.ai_busy:
            return
        # In AI mode, block clicks when it's Black's turn
        if self.ai_mode and eng.turn() == 'B':
            return

        r, c = self._rc(event.x, event.y)
        if not (0 <= r < 8 and 0 <= c < 8):
            return

        bd   = eng.board()
        sym  = bd[r][c]
        turn = eng.turn()
        own  = (sym.isupper() and turn == 'W') or (sym.islower() and sym != '.' and turn == 'B')

        if self.selected is None:
            if own:
                self.selected = (r, c)
                self.valid    = eng.valid_moves(r, c)
        else:
            if (r, c) in self.valid:
                self._execute_move(*self.selected, r, c, bd, player=True)
                return
            elif own:
                self.selected = (r, c)
                self.valid    = eng.valid_moves(r, c)
            else:
                self.selected = None
                self.valid    = []

        self._draw()
        self._update_status()

    def _execute_move(self, fr: int, fc: int, tr: int, tc: int,
                      bd: list, player: bool = False):
        """Commit a move, handle promotion dialog if needed."""
        eng   = self.engine
        sym   = bd[fr][fc]
        promo = 'Q'
        if sym.upper() == 'P' and (tr == 7 or tr == 0):
            if player:
                promo = self._ask_promotion(eng.turn())
            # AI always promotes to queen (already encoded in the AI's choice)

        eng.make_move(fr, fc, tr, tc, promo)
        self.selected = None
        self.valid    = []
        self.ai_last  = None if player else (fr, fc, tr, tc)

        self._draw()
        self._update_status()

        if eng.is_game_over():
            self._show_game_over()
            return

        # If AI mode and it's now Black's turn, fire off the AI
        if self.ai_mode and eng.turn() == 'B' and not eng.is_game_over():
            self._trigger_ai()

    # AI
    def _trigger_ai(self):
        """Run the AI search in a background thread; apply result on main thread."""
        self.ai_busy = True
        self._update_status()

        def worker():
            result = self.engine.get_best_move('B', AI_DEPTH)
            # Schedule GUI update back on the main thread
            self.root.after(0, lambda: self._apply_ai_move(result))

        threading.Thread(target=worker, daemon=True).start()

    def _apply_ai_move(self, result):
        self.ai_busy = False
        if result is None or self.engine.is_game_over():
            self._update_status()
            return
        fr, fc, tr, tc, promo = result
        bd = self.engine.board()
        self.ai_last = (fr, fc, tr, tc)
        self.engine.make_move(fr, fc, tr, tc, promo)
        self.selected = None
        self.valid    = []
        self._draw()
        self._update_status()
        if self.engine.is_game_over():
            self._show_game_over()

    #  Helpers ─
    def _show_game_over(self):
        eng = self.engine
        msg = (
            "It's a draw by stalemate!"
            if eng.is_draw()
            else f"{'White' if eng.winner() == 'W' else 'Black'} wins by checkmate!"
        )
        messagebox.showinfo("Game Over", msg)

    def _toggle_ai(self):
        self.ai_mode = not self.ai_mode
        self.ai_btn_var.set("AI: ON" if self.ai_mode else "AI: OFF")
        self.ai_last = None
        self._update_status()
        self._draw()
        # If AI is now on and it's already Black's turn, trigger it
        if self.ai_mode and self.engine.turn() == 'B' and not self.engine.is_game_over():
            self._trigger_ai()

    def _new_game(self):
        self.engine.reset()
        self.selected = None
        self.valid    = []
        self.ai_last  = None
        self.ai_busy  = False
        self._update_status()
        self._draw()

    def _ask_promotion(self, color: str) -> str:
        win = tk.Toplevel(self.root)
        win.title("Promote pawn")
        win.resizable(False, False)
        win.grab_set()
        win.transient(self.root)

        tk.Label(win, text="Choose promotion piece:",
                 font=("Arial", 12, "bold"), pady=8).pack()
        frame = tk.Frame(win)
        frame.pack(padx=10, pady=(0, 10))

        result = ['Q']
        names  = {'Q': 'Queen', 'R': 'Rook', 'B': 'Bishop', 'N': 'Knight'}

        for pt in ('Q', 'R', 'B', 'N'):
            uni = UNICODE[pt if color == 'W' else pt.lower()]
            def pick(p=pt):
                result[0] = p
                win.destroy()
            tk.Button(
                frame, text=f"{uni}\n{names[pt]}",
                font=("Arial", 18), width=4, height=2,
                command=pick, relief='groove', cursor='hand2'
            ).pack(side='left', padx=4)

        self.root.update_idletasks()
        bx = self.root.winfo_rootx() + self.root.winfo_width()  // 2
        by = self.root.winfo_rooty() + self.root.winfo_height() // 2
        win.update_idletasks()
        win.geometry(f"+{bx - win.winfo_width()//2}+{by - win.winfo_height()//2}")
        self.root.wait_window(win)
        return result[0]


#  Entry point ─
def main():
    root = tk.Tk()
    ChessGUI(root)
    root.mainloop()


if __name__ == '__main__':
    main()
