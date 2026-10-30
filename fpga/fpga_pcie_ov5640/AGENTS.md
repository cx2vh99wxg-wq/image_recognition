# FPGA 烧录文件输出约定

用户于 2026-10-30 明确要求：以后最终交付的 `.sbit`、`.sfc` 烧录文件统一保存到：

`D:\Study_Object\image_recognition\image_recognition\fpga\fpga_pcie_ov5640\project\generate_bitstream`

独立工程的工具中间输出可以保留在其自身目录，最终可烧录文件须发布到上述目录。
文件名须清楚区分 M/S 端和用途，不能用 M 端版本覆盖原有 S 端的通用文件。
