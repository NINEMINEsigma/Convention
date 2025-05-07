
import os
import sys
sys.path.append(os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

from Convention.Visual.Core import *

def run():
    icon_config = ImageConvertConfig(
        ico=ICOConvertConfig(name="favicon", size=(48, 48)),
        log_call=lambda x: print(x),
    )

    image = ImageObject(r"E:\ninemine-lobe-chat\public\liubai.png")


    result = icon_config.convert_to(
        input=image,
        output_file_name="favicon.ico",
        output_dir="./tests/favicon_dir/",
        must_output_dir_exist=True,
        callback=lambda x: print(x),
    )

if __name__ == "__main__":
    run()

