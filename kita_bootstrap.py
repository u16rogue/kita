import requests
import os
import shutil
import zipfile

os.chdir(os.path.dirname(os.path.realpath(__file__)))

# kita bootstrapper configuration

cfg_imgui_extract_file = [
    "LICENSE.txt",
    "imconfig.h",
    "imgui.cpp",
    "imgui.h",
    "imgui_draw.cpp",
    "imgui_internal.h",
    "imgui_tables.cpp",
    "imgui_widgets.cpp",
    "imstb_rectpack.h",
    "imstb_textedit.h",
    "imstb_truetype.h",
    "imgui_impl_glfw.cpp",
    "imgui_impl_glfw.h",
    "imgui_impl_opengl3.h",
    "imgui_impl_opengl3.cpp",
    "imgui_impl_opengl3_loader.h"
]

cfg_imgui_dir = "deps/imgui_kita/imgui"
cfg_zip_file  = f"{cfg_imgui_dir}/_latest.zip"

# ----------------------------------------------------------------------------------------------------------------------------------------------------------------

def bootstrap_imgui():
    print("[+] Grabbing latest release of ImGui from the GitHub repository")
    r_repo = requests.get("https://api.github.com/repos/ocornut/imgui/releases/latest")
    if r_repo.status_code != 200:
        print(f"[!] Request did not return OK! (status_code: {r_repo.status_code})")
        exit(1)
    if os.path.exists(cfg_imgui_dir):
        print("[+] Deleting imgui directory")
        shutil.rmtree(cfg_imgui_dir)
    os.mkdir(cfg_imgui_dir)
    zip_url = r_repo.json()["zipball_url"]
    print("[+] Downloading zip:", zip_url)
    zip_dl = requests.get(zip_url, allow_redirects=True)
    if zip_dl.status_code != 200:
        print(f"[!] Failed to download latest release (status_code: {zip_dl.status_code})")
        exit(1)
    print("[+] Writing contents")
    open(cfg_zip_file, "wb").write(zip_dl.content)
    with zipfile.ZipFile(cfg_zip_file, "r") as zip:
        for zf in zip.filelist:
            for tf in cfg_imgui_extract_file:
                if tf in zf.filename:
                    out = f'{cfg_imgui_dir}/{tf}'
                    print("[+] Extracting:", zf.filename, "to", out)
                    open(out, 'wb').write(zip.read(zf.filename))
                    cfg_imgui_extract_file.remove(tf)
    if len(cfg_imgui_extract_file) != 0:
        print("[!] WARNING: The following files were not extracted:")
        for i in cfg_imgui_extract_file:
            print("\t*", i)
    print("[+] Cleaning up")
    os.remove(cfg_zip_file)

# ----------------------------------------------------------------------------------------------------------------------------------------------------------------

print("kita bootstrapper")
print("[#] Bootstrapping ImGui")
bootstrap_imgui()
print("[+] Done!")