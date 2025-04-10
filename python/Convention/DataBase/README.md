# DataBase 模块

DataBase模块提供了数据库操作接口，目前主要支持SQLite数据库。

## 目录结构

- `AbsInterface.py`: 数据库抽象接口
- `light_sqlite.py`: SQLite数据库实现
- `Core.py`: 预留的核心功能扩展

## 功能特性

### 1. 数据库抽象接口 (AbsInterface.py)

#### 1.1 查询构建器

- 支持链式调用构建SQL查询
- 提供常用SQL命令的封装
  - SELECT
  - INSERT
  - UPDATE
  - DELETE
  - CREATE DATABASE
  - DROP DATABASE
  - CREATE TABLE
  - DROP TABLE
  - CREATE INDEX
  - DROP INDEX

#### 1.2 查询条件

- WHERE子句
- ORDER BY子句
- 支持升序和降序排序

### 2. SQLite实现 (light_sqlite.py)

#### 2.1 数据库连接

- 支持内存数据库
- 支持文件数据库
- 自动连接管理

#### 2.2 数据操作

- 插入数据
- 更新数据
- 删除数据
- 查询数据
- 事务支持
- 批量操作

#### 2.3 表管理

- 创建表
- 删除表
- 表结构管理

## 使用示例

### 1. 基本使用示例

```python
from Convention.DataBase import light_sqlite

# 创建数据库连接
db = light_sqlite.light_core("example.db")

# 创建表
db.create_table("users", {
    "id": "INTEGER PRIMARY KEY",
    "name": "TEXT",
    "age": "INTEGER"
})

# 插入数据
db.insert("users", {
    "name": "张三",
    "age": 25
})

# 查询数据
cursor = db.select("users", {"name": "张三"})
for row in cursor:
    print(row["name"], row["age"])
```

### 2. 使用查询构建器

```python
from Convention.DataBase import query

# 构建查询
q = query.command.select("users", ["name", "age"]) + \
    query.word.WHERE() + "age > 18" + \
    query.word.ORDER_BY() + "age" + query.word.descending()

# 执行查询
result = db.execute(str(q))
```

### 3. 事务处理

```python
# 使用事务
def update_user():
    db.update("users", {"age": 26})
    db.update("users", {"name": "李四"})

db.execute_transaction(update_user)
```

## 依赖项

- sqlite3: Python内置SQLite支持

## 注意事项

1. 使用事务时注意异常处理
2. 批量操作时注意内存使用
3. 文件数据库注意文件权限

## 性能优化

1. 使用索引提高查询性能
2. 合理使用事务减少I/O操作
3. 批量操作时使用execute_many

## 贡献指南

欢迎提交Issue和Pull Request来改进功能或添加新特性。
