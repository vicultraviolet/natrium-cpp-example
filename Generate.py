import enum
import sys
import os

class BuildSystem(enum.IntEnum):
	Makefiles = 1
	MsBuild   = 2
	CodeLite  = 3
	Clean     = 4

def build_system_to_str(build_system: BuildSystem) -> str:
	match(build_system):
		case BuildSystem.Makefiles:
			return "make"
		case BuildSystem.MsBuild:
			return "msbuild"
		case BuildSystem.CodeLite:
			return "codelite"
		case BuildSystem.Clean:
			return "codelite"
		case _:
			return ""

class BuildSystemInfo:
	def __init__(self, info: str, build_command: str): 
		self.info = info
		self.build_command = build_command
	
	info: str
	build_command: str

def main():
	premake: str = ""
	if sys.platform.startswith("linux"):
		premake = "./natrium-cpp/dependencies/premake/bin/linux/premake5"
	elif sys.platform.startswith("win32"):
		premake = ".\\natrium-cpp\\dependencies\\premake\\bin\\windows\\premake5.exe"

	build_systems: list[BuildSystemInfo] = []
	build_systems.insert(0, BuildSystemInfo("", ""))
	
	build_systems.insert(BuildSystem.Makefiles, BuildSystemInfo(
		"Generate GNU Makefiles",
		f"{premake} gmake"
	))
	build_systems.insert(BuildSystem.MsBuild, BuildSystemInfo(
		"Generate MsBuild project files",
		f"{premake} vs2022"
	))
	build_systems.insert(BuildSystem.CodeLite, BuildSystemInfo(
		"Generate CodeLite project files",
		f"{premake} codelite"
	))
	build_systems.insert(BuildSystem.Clean, BuildSystemInfo(
		"Clean generated files",
		f"{premake} clean"
	))

	match (sys.argv[1]):
		case "make":
			os.system(build_systems[BuildSystem.Makefiles].build_command)
		case "msbuild":
			os.system(build_systems[BuildSystem.MsBuild].build_command)
		case "codelite":
			os.system(build_systems[BuildSystem.CodeLite].build_command)
		case "clean":
			os.system(build_systems[BuildSystem.Clean].build_command)
		case _:
			print("Options: ")
			for i, build_system in enumerate(build_systems):
				if i == 0:
					continue
				print(f"\t{build_system_to_str(i)}: {build_system.info}")
			

if __name__ == "__main__":
	main()