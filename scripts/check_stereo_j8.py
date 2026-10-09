"""Check J8 camera pin mapping, project source closure, and generated top isolation."""
from pathlib import Path
import re
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
p = root / "fpga/fpga_pcie_ov5640/stereo_j8"
fdc = (p / "stereo_j8.fdc").read_text(encoding="utf-8")
loc = dict(re.findall(r"define_attribute \{p:(.*?)\} \{PAP_IO_LOC\} \{(.*?)\}", fdc))
# Each tuple is (module connector J17 = board J8 pin number, FPGA ball).
# Independently transcribed from module schematic + board manual PDF pp24-25.
expected = {
    "cmos5_sda": (4,"P14"), "cmos5_scl": (6,"R14"),
    "cmos5_reset": (12,"V18"), "cmos5_vsync": (17,"W17"),
    "cmos5_href": (9,"N15"), "cmos5_pclk": (18,"T18"),
    "cmos6_sda": (20,"N17"), "cmos6_scl": (23,"U20"),
    "cmos6_reset": (35,"AA21"), "cmos6_vsync": (34,"R17"),
    "cmos6_href": (33,"U18"), "cmos6_pclk": (32,"P16"),
}
for n,pins,balls in [(5,[8,11,13,14,7,5,10,3],"Y18 AA19 AB20 V19 L13 AB22 Y19 AB21"),
                     (6,[31,25,26,24,27,29,22,21],"U17 V20 R16 P15 W19 W20 P17 AB18")]:
    for bit,(pin,ball) in enumerate(zip(pins,balls.split())):
        expected[f"cmos{n}_data[{bit}]"] = (pin,ball)
for name,(_,ball) in expected.items():
    assert loc.get(name) == ball, (name,ball,loc.get(name))
assert len(set(loc.values())) == len(loc), "duplicate ball assignment"
assert not re.search(r"p:cmos2_|p:RXD|p:spi_",fdc)
top = (p / "image_pcie_capture.v").read_text(encoding="utf-8")
assert "cmos2_" not in top and "u_cnn_top" not in top and "u_uart_lcd" not in top
assert ".wframe_data0(video5_data)" in top
assert ".wframe_data1(16'h0000)" in top
assert ".wframe_data2(video6_data)" in top
count=0
for entry in (p / "design_files.txt").read_text().splitlines():
    file=p/entry
    assert file.is_file(), file
    assert not file.name.startswith("test_"), "simulation stubs must not enter synthesis"
    if file.suffix == ".idf":
        for e in ET.parse(file).findall(".//file_list/source/file"):
            assert (file.parent / e.attrib["pathname"]).is_file(), e.attrib["pathname"]
            count+=1
print(f"check_stereo_j8: PASS (28 camera pins, {len(loc)} unique balls, {count} IP source paths, isolated camera top)")
