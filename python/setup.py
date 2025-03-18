from setuptools import setup, find_packages
import io

setup(
    name="Convention",
    version="0.1.3",
    author="Convention Unity",
    description="A comprehensive Python utility library",
    long_description=io.open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/yourusername/Convention",  # 请替换为实际的GitHub仓库地址
    packages=find_packages(),
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: MIT License",
        "Operating System :: OS Independent",
    ],
    python_requires=">=3.12",
    install_requires=[
        # 在这里添加依赖包
    ],
)