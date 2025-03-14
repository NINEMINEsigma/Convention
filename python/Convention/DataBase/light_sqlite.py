from typing                         import *
from abc                            import *
import sqlite3                      as     base

from ..DataBase.AbsInterface    import *

memory_db:str = ":memory:"

class light_core(abs_db):
    def __init__(self, name:str=memory_db):
        self.connect_to(name)
        self.connection:  base.Connection    = None
    def __del__(self):
        self.close()

    @property
    def name(self):
        return self.__my_database_name

    def connect(self):
        """连接到数据库"""
        self.connection = base.connect(self.__my_database_name)
        self.connection.row_factory = base.Row
        return self
    def connect_to(self, name:str=memory_db):
        self.__my_database_name = name
        self.connect()
        return self
    def close(self):
        """关闭数据库连接"""
        if self.connection:
            self.connection.close()
        return self

    @override
    def execute(self, query:str, params=None) -> base.Cursor:
        """执行SQL查询"""
        cursor = self.connection.cursor()
        if params:
            cursor.execute(query, params)
        else:
            cursor.execute(query)
        self.connection.commit()
        return cursor
    def execute_many(self, query:str, params:List[Tuple]) -> base.Cursor:
        """执行多个SQL查询"""
        cursor = self.connection.cursor()
        cursor.executemany(query, params)
        self.connection.commit()
        return cursor
    def execute_script(self, script:str) -> base.Cursor:
        """执行SQL脚本"""
        cursor = self.connection.cursor()
        cursor.executescript(script)
        self.connection.commit()
        return cursor
    def execute_transaction(self, func:Callable) -> base.Cursor:
        """执行事务"""
        cursor = self.connection.cursor()
        self.connection.execute("BEGIN TRANSACTION")
        try:
            func(cursor)
            self.connection.commit()
        except Exception as e:
            self.connection.rollback()
            raise e
        return cursor
    def execute_transaction_many(self, func:Callable) -> base.Cursor:
        """执行多个事务"""
        cursor = self.connection.cursor()
        self.connection.execute("BEGIN TRANSACTION")
        try:
            func(cursor)
            self.connection.commit()
        except Exception as e:
            self.connection.rollback()
            raise e
        return cursor
    def execute_transaction_script(self, script:str) -> base.Cursor:
        """执行事务脚本"""
        cursor = self.connection.cursor()
        self.connection.execute("BEGIN TRANSACTION")
        try:
            cursor.executescript(script)
            self.connection.commit()
        except Exception as e:
            self.connection.rollback()
            raise e
        return cursor

    def fetch_all(self, query:str, params=None):
        """获取所有查询结果"""
        cursor = self.execute(query, params)
        return cursor.fetchall()

    def fetch_one(self, query:str, params=None):
        """获取单个查询结果"""
        cursor = self.execute(query, params)
        return cursor.fetchone()

    def create_table(self, table_name:str, columns:dict):
        """创建表"""
        columns_str = ", ".join([f"{name} {type_}" for name, type_ in columns.items()])
        query = f"CREATE TABLE IF NOT EXISTS {table_name} ({columns_str})"
        self.execute(query)
        return self

def internal_exmple():
    # 示例使用
    db = light_core("example.db")  # 指定数据库文件名
    db.connect()

    # 创建表
    db.create_table("users", {
        "id": "INTEGER PRIMARY KEY",
        "name": "TEXT NOT NULL",
        "age": "INTEGER"
    })

    # 插入数据
    db.execute("INSERT INTO users (name, age) VALUES (?, ?)", ("Alice", 30))
    db.execute("INSERT INTO users (name, age) VALUES (?, ?)", ("Bob", 25))

    # 查询数据
    users = db.fetch_all("SELECT * FROM users")
    for user in users:
        print(dict(user))

    db.close()

    # 重新连接到同一个数据库文件
    db = light_core("example.db")
    db.connect()

    # 再次查询数据
    users = db.fetch_all("SELECT * FROM users")
    for user in users:
        print(dict(user))

    db.close()

if __name__ == "__main__":
    internal_exmple()

















