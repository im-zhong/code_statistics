from conan import ConanFile
from conan.tools.cmake import CMakeToolchain, CMake, cmake_layout, CMakeDeps
import os


class mycppRecipe(ConanFile):
    name = "CodeStatistics"
    version = "0.2.0"
    package_type = "application"

    # Optional metadata
    license = "<Put the package license here>"
    author = "im.zhong@outlook.com"
    url = "https://github.com/im-zhong/code_statistics"
    description = "A simple code statistics tool"
    topics = ("C++", "Code", "Statistics")

    # Binary configuration
    settings = "os", "compiler", "build_type", "arch"

    # Sources are located in the same place as this recipe, copy them to the recipe
    exports_sources = "CMakeLists.txt", "src/*"

    def requirements(self: "ConanFile"):
        self.requires("cli11/2.3.2")
        self.requires("doctest/2.4.11")
        self.requires("fmt/10.2.1")

    def layout(self):
        cmake_layout(self)

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        tc = CMakeToolchain(self)
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()
        # Create a symlink to the compile_commands.json file
        db_path = os.path.join(
            "build", str(self.settings.build_type), "compile_commands.json")
        if os.path.exists("../../compile_commands.json"):
            os.unlink("../../compile_commands.json")
        os.symlink(db_path, "../../compile_commands.json")

    def package(self):
        cmake = CMake(self)
        cmake.install()
