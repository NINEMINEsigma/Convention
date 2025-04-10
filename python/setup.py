from setuptools import setup, find_packages
import io

setup(
    name="Convention",
    version="0.3.3",
    author="LiuBai",
    description="A comprehensive Python utility library",
    long_description=io.open("README.md", encoding="utf-8").read(),
    long_description_content_type="text/markdown",
    url="https://github.com/NINEMINEsigma/Convention",
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