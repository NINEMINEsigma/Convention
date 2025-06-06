from typing                         import *
from abc                            import *
import sqlite3                      as     base

from .AbsInterface    import *

memory_db:str = ":memory:"

class light_core(abs_db):
    def __init__(self, name:str=memory_db):
        self.connection:  base.Connection    = None
        self.connect_to(name)
    def __del__(self):
        self.close()

    @property
    def name(self):
        return self._my_database_name

    def insert(self, table_name:str, data:dict) -> base.Cursor:
        """插入数据"""
        cursor = self.connection.cursor()
        cursor.execute(f"INSERT INTO {table_name} ({', '.join(data.keys())}) VALUES ({', '.join([f"'{data[key]}'" for key in data.keys()])})")
        self.connection.commit()
        return cursor

    def update(self, table_name:str, data:dict) -> base.Cursor:
        """更新数据"""
        cursor = self.connection.cursor()
        if data:
            cursor.execute(f"UPDATE {table_name} SET {', '.join([f"{key} = '{data[key]}'" for key in data.keys()])}")
        else:
            raise ValueError("更新数据时data字典不能为空")
        self.connection.commit()
        return cursor

    def delete(self, table_name:str, data:dict) -> base.Cursor:
        """删除数据"""
        cursor = self.connection.cursor()
        if data:
            cursor.execute(f"DELETE FROM {table_name} WHERE {' AND '.join([f"{key} = '{data[key]}'" for key in data.keys()])}")
        else:
            raise ValueError("删除数据时data字典不能为空")
        self.connection.commit()
        return cursor

    def select(self, table_name:str, data:dict) -> base.Cursor:
        """查询数据"""
        cursor = self.connection.cursor()
        if data:
            cursor.execute(f"SELECT * FROM {table_name} WHERE {' AND '.join([f"{key} = '{data[key]}'" for key in data.keys()])}")
        else:
            cursor.execute(f"SELECT * FROM {table_name}")
        return cursor

    def contains(self, table_name:str, data:dict) -> bool:
        """检查数据是否存在"""
        cursor = self.connection.cursor()
        if data:
            cursor.execute(f"SELECT * FROM {table_name} WHERE {' AND '.join([f"{key} = '{data[key]}'" for key in data.keys()])}")
        else:
            cursor.execute(f"SELECT * FROM {table_name} LIMIT 1")
        return cursor.fetchone() is not None
    
    def contains_key(self, table_name:str, key_column:str, key_value) -> bool:
        """检查主键是否存在
        
        Args:
            table_name (str): 表名
            key_column (str): 主键列名
            key_value: 主键值
            
        Returns:
            bool: 如果主键存在返回True，否则返回False
        """
        cursor = self.connection.cursor()
        # 使用参数化查询以防止SQL注入
        cursor.execute(f"SELECT 1 FROM {table_name} WHERE {key_column} = ?", (key_value,))
        return cursor.fetchone() is not None

    def connect(self):
        """连接到数据库"""
        self.connection = base.connect(self._my_database_name)
        self.connection.row_factory = base.Row
        return self
    def connect_to(self, name:str=memory_db):
        self._my_database_name = name
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
    
    def contains_table(self, table_name:str) -> bool:
        """检查表是否存在"""
        cursor = self.connection.cursor()
        cursor.execute(f"SELECT name FROM sqlite_master WHERE type='table' AND name='{table_name}'")
        return cursor.fetchone() is not None

def internal_exmple(db_name:str = "example.db"):
    # 示例使用
    db = light_core(db_name)  # 指定数据库文件名

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
    db = light_core(db_name)
    db.connect()

    # 再次查询数据
    users = db.fetch_all("SELECT * FROM users")
    for user in users:
        print(dict(user))

    db.close()

if __name__ == "__main__":
    internal_exmple()

















