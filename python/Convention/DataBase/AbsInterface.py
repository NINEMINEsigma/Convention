from typing import *
from abc import *
from ..Str.Core import UnWrapper as UnWrapper2Str

class abs_query_item(Callable[[], str], ABC):
    @abstractmethod
    def __call__(self) -> str:
        return ""

    def __str__(self):
        return self()

def UnWrapper(query_item:Union[str, abs_query_item]) -> str:
    if isinstance(query_item, abs_query_item):
        return query_item()
    else:
        return UnWrapper2Str(query_item)

class abs_db(Callable[[str, Any], Any], ABC):
    @abstractmethod
    def execute(self, query:str, params=None):
        return None

class query:
    class command:
        class use(abs_query_item):
            def __init__(
                self,
                database:                       str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.database:                  str         = database
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{"USE" if self.is_bigger_char_command else "use"} {UnWrapper(self.database)}"

        class select(abs_query_item):
            def __init__(
                self,
                table:                          str,
                columns:                        List[str]   = None,
                is_bigger_char_command = False
                ) -> None:
                self.table:                     str         = table
                self.columns:                   List[str]   = columns if columns else ["*"]
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                columns_str = ", ".join(self.columns)
                return f"{'SELECT' if self.is_bigger_char_command else 'select'} {columns_str} FROM {self.table}"

        class insert(abs_query_item):
            def __init__(
                self,
                table: str,
                columns: List[str],
                values: List[Any],
                is_bigger_char_command = False
                ) -> None:
                self.table:                     str         = table
                self.columns:                   List[str]   = columns
                self.values:                    List[Any]   = values
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                columns_str = ", ".join(self.columns)
                values_str = ", ".join([f"'{value}'" for value in self.values])
                return f"{'INSERT' if self.is_bigger_char_command else 'insert'} INTO {self.table} ({columns_str}) VALUES ({values_str})"

        class update(abs_query_item):
            def __init__(
                self,
                table:                          str,
                updates:                        Dict[str, Any],
                condition:                      str         = None,
                is_bigger_char_command = False
                ) -> None:
                self.table:                     str         = table
                self.updates:                   Dict[str, Any] = updates
                self.condition:                 str         = condition
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                updates_str = ", ".join([f"{column} = '{str(value)}'" for column, value in self.updates.items()])
                query = f"{'UPDATE' if self.is_bigger_char_command else 'update'} {self.table} SET {updates_str}"
                if self.condition:
                    query += f" WHERE {self.condition}"
                return query

        class delete(abs_query_item):
            def __init__(
                self,
                table:                          str,
                condition:                      str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.table:                     str         = table
                self.condition:                 str         = condition
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{'DELETE' if self.is_bigger_char_command else 'delete'} FROM {self.table} WHERE {self.condition}"

        class create_db(abs_query_item):
            def __init__(
                self,
                name:                           str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{'CREATE' if self.is_bigger_char_command else 'create'} DATABASE {self.name}"
        class drop_db(abs_query_item):
            def __init__(
                self,
                name:                           str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{'DROP' if self.is_bigger_char_command else 'drop'} DATABASE {self.name}"
        class after_db(abs_query_item):
            def __init__(
                self,
                name:                           str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"ALTER DATABASE {self.name}"

        class create_table(abs_query_item):
            def __init__(
                self,
                name:                           str,
                columns:                        Dict[str, str],
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.columns:                   Dict[str, str] = columns
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                columns_str = ", ".join([f"{column} {data_type}" for column, data_type in self.columns.items()])
                return f"{'CREATE' if self.is_bigger_char_command else 'create'} TABLE {self.name} ({columns_str})"
        class drop_table(abs_query_item):
            def __init__(
                self,
                name:                           str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{'DROP' if self.is_bigger_char_command else 'drop'} TABLE {self.name}"
        class after_table(abs_query_item):
            def __init__(
                self,
                name:                           str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"ALTER TABLE {self.name}"

        class create_index(abs_query_item):
            def __init__(
                self,
                name:                           str,
                table:                          str,
                columns:                        List[str],
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.table:                     str         = table
                self.columns:                   List[str]   = columns
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{'CREATE' if self.is_bigger_char_command else 'create'} INDEX {self.name} ON {self.table} ({', '.join(self.columns)})"
        class drop_index(abs_query_item):
            def __init__(
                self,
                name:                           str,
                is_bigger_char_command:         bool        = False
                ) -> None:
                self.name:                      str         = name
                self.is_bigger_char_command:    bool        = is_bigger_char_command

            @override
            def __call__(self) -> str:
                return f"{'DROP' if self.is_bigger_char_command else 'drop'} INDEX {self.name}"

    class symbol:
        class end(abs_query_item):
            @override
            def __call__(self) -> str:
                return ";"

        class any(abs_query_item):
            @override
            def __call__(self) -> str:
                return "*"

        class target(abs_query_item):
            @override
            def __call__(self) -> str:
                return "?"

    class word:
        class FROM(abs_query_item):
            @override
            def __call__(self) -> str:
                return "FROM"
        class WHERE(abs_query_item):
            @override
            def __call__(self) -> str:
                return "WHERE"
        class ORDER(abs_query_item):
            @override
            def __call__(self) -> str:
                return "ORDER"
        class BY(abs_query_item):
            @override
            def __call__(self) -> str:
                return "BY"
        class ORDER_BY(abs_query_item):
            @override
            def __call__(self) -> str:
                return "ORDER BY"

        class ascending(abs_query_item):
            @override
            def __call__(self) -> str:
                return "ASC"
        class descending(abs_query_item):
            @override
            def __call__(self) -> str:
                return "DESC"

    class encoding:
        class utf8(abs_query_item):
            @override
            def __call__(self) -> str:
                return "utf8"

def make_query(*args) -> str:
    result = ""
    last_item = None
    for item in args:
        result += UnWrapper2Str(item) + " "
        last_item = item
    if isinstance(last_item, query.symbol.end) is False:
        result += ";"
    return result

if __name__ == "__main__":
    print(make_query(query.command.insert("users",["name","age"],[query.symbol.target(), query.symbol.target()])))

