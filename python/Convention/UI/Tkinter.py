from typing import *
import tkinter as base

grid_row        = int
grid_column     = int
grid_padx       = int
grid_pady       = int
grid_position   = Union[Tuple[grid_row, grid_column, grid_padx, grid_pady], Sequence[int]]

class light_tk_window:
    def __init__(self):
        self.window:    base.Tk = base.Tk()
        self.__title:   str     = "window"
        
    def configure(
        self,
        cnf:    Optional[Dict[str, Any]]    = None,
        **kwargs
        ) -> Optional[Dict[str, tuple[str, str, str, Any, Any]]]:
        return self.window.configure(cnf, **kwargs)
    def set_background_color(self, color):
        self.window.configure(bg=color)
    def set_foreground_color(self, color):
        self.window.configure(fg=color)

    @property
    def title(self):
        return self.__title
    @title.setter
    def title(self, title:str):
        self.__title = title
        self.window.title(title)
        return self.__title
    def set_title(self, title:str):
        self.title = title
        return self
    
    def build_element(
        self,
        typen:  type,
        *,
        grid:   Optional[grid_position]     = None,
        **kwargs
        ):
        element:base.Widget = typen(self.window, **kwargs)
        if grid is not None:
            element.grid(row=grid[0], column=grid[1], padx=grid[2], pady=grid[3])
        return element
    def make_button(self, grid:Optional[grid_position]=None, **kwargs) -> base.Button:
        return self.build_element(base.Button, grid=grid, **kwargs)
    def make_inputfield(self, grid:Optional[grid_position]=None, **kwargs) -> base.Entry:
        return self.build_element(base.Entry, grid=grid, **kwargs)
    def make_label(self, grid:Optional[grid_position]=None, **kwargs) -> base.Label:
        return self.build_element(base.Label, grid=grid, **kwargs)
    def make_text(self, grid:Optional[grid_position]=None, **kwargs) -> base.Text:
        return self.build_element(base.Text, grid=grid, **kwargs)
    def make_canvas(
        self,
        width:float,
        height:float, 
        grid:Optional[grid_position]=None,
        **kwargs
        ) -> base.Canvas:
        return self.build_element(base.Canvas, width=width, height=height, grid=grid, **kwargs)
    def make_frame(self, grid:Optional[grid_position]=None, **kwargs) -> base.Frame:
        return self.build_element(base.Frame, grid=grid, **kwargs)
    def make_menu(self, grid:Optional[grid_position]=None, **kwargs) -> base.Menu:
        return self.build_element(base.Menu, grid=grid, **kwargs)
    def make_menuitem(self, grid:Optional[grid_position]=None, **kwargs) -> base.Menubutton:
        return self.build_element(base.Menu, grid=grid, **kwargs)
    def make_radiobutton(self, grid:Optional[grid_position]=None, **kwargs) -> base.Radiobutton:
        return self.build_element(base.Radiobutton, grid=grid, **kwargs)
    def make_scale(self, grid:Optional[grid_position]=None, **kwargs) -> base.Scale:
        return self.build_element(base.Scale, grid=grid, **kwargs)
    def make_scrollbar(self, grid:Optional[grid_position]=None, **kwargs) -> base.Scrollbar:
        return self.build_element(base.Scrollbar, grid=grid, **kwargs)
    def make_spinbox(self, grid:Optional[grid_position]=None, **kwargs) -> base.Spinbox:
        return self.build_element(base.Spinbox, grid=grid, **kwargs)
    def make_listbox(self, grid:Optional[grid_position]=None, **kwargs) -> base.Listbox:
        return self.build_element(base.Listbox, grid=grid, **kwargs)
    def make_toplevel(self, grid:Optional[grid_position]=None, **kwargs) -> base.Toplevel:
        return self.build_element(base.Toplevel, grid=grid, **kwargs)
    def make_window(self, grid:Optional[grid_position]=None, **kwargs) -> base.Tk:
        return self.build_element(base.Tk, grid=grid, **kwargs)
    def make_widget(self, grid:Optional[grid_position]=None, **kwargs) -> base.Widget:
        return self.build_element(base.Widget, grid=grid, **kwargs)
    def make_checkbutton(self, grid:Optional[grid_position]=None, **kwargs) -> base.Checkbutton:
        return self.build_element(base.Checkbutton, grid=grid, **kwargs)

    

    def mainloop(self, n:int=0):
        self.window.mainloop(n)
