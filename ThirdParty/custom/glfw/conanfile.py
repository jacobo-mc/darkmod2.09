import os
import glob
import shutil
from conans import ConanFile, CMake, tools
from conans.errors import ConanException


class GlfwConan(ConanFile):
    name = "glfw"
    version = "3.3.2"
    description = "GLFW is a free, Open Source, multi-platform library for OpenGL, OpenGL ES and Vulkan" \
                  "application development. It provides a simple, platform-independent API for creating" \
                  "windows, contexts and surfaces, reading input, handling events, etc."
    settings = "os", "arch", "build_type", "compiler"
    license = "Zlib"
    url = "https://github.com/conan-io/conan-center-index"
    homepage = "https://github.com/glfw/glfw"
    topics = ("conan", "gflw", "opengl", "vulkan", "opengl-es")
    generators = "cmake"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "no_opengl": [True, False],
        "no_xorg": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "no_opengl": False,
        "no_xorg": False,
    }
    _source_subfolder = "source_subfolder"
    _build_subfolder = "build_subfolder"
    _cmake = None
    exports_sources = ["CMakeLists.txt"]

    def _configure_cmake(self):
        if not self._cmake:
            self._cmake = CMake(self)
            self._cmake.definitions["GLFW_BUILD_EXAMPLES"] = False
            self._cmake.definitions["GLFW_BUILD_TESTS"] = False
            self._cmake.definitions["GLFW_BUILD_DOCS"] = False
            if self.settings.compiler == "Visual Studio":
                self._cmake.definitions["USE_MSVC_RUNTIME_LIBRARY_DLL"] = "MD" in self.settings.compiler.runtime
            self._cmake.configure(source_folder=self._source_subfolder)
        return self._cmake

    def requirements(self):
        # stgatilov: Unfortunately, conan is not very good at deciding
        # which system packages to install in every version of every distro.
        # Since TDM links both packages directly, we can just skip system packages here
        if not self.options.no_opengl:
            self.requires("opengl/system")
        if self.settings.os == "Linux":
            if not self.options.no_xorg:
                self.requires("xorg/system")

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def configure(self):
        if self.options.shared:
            del self.options.fPIC
        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd

    def source(self):
        #tools.get(**self.conan_data["sources"][self.version])
        tools.get("https://github.com/glfw/glfw/archive/{}.zip".format(self.version))
        extracted_dir = "{}-{}".format(self.name, self.version)
        os.rename(extracted_dir, self._source_subfolder)
        # stgatilov: pass conan settings (like -m32) to CMake
        os.rename(os.path.join(self._source_subfolder, "CMakeLists.txt"), os.path.join(self._source_subfolder, "CMakeLists_original.txt"))
        shutil.copy("CMakeLists.txt", os.path.join(self._source_subfolder, "CMakeLists.txt"))

    def build(self):
        cmake = self._configure_cmake()
        cmake.build()
        if self.settings.os == "Macos" and self.options.shared:
            with tools.chdir(os.path.join(self._source_subfolder, 'src')):
                for filename in glob.glob('*.dylib'):
                    self.run('install_name_tool -id {filename} {filename}'.format(filename=filename))

    def package(self):
        self.copy("LICENSE*", dst="licenses", src=self._source_subfolder)
        cmake = self._configure_cmake()
        cmake.install()
        tools.rmdir(os.path.join(self.package_folder, "lib", "cmake"))
        tools.rmdir(os.path.join(self.package_folder, "lib", "pkgconfig"))

    def package_info(self):
        self.cpp_info.names["pkg_config"] = "glfw3"
        self.cpp_info.libs = tools.collect_libs(self)
        if self.settings.os == "Linux":
            self.cpp_info.system_libs.extend(["m", "pthread", "dl", "rt"])
        elif self.settings.os == "Macos":
            self.cpp_info.frameworks.extend(["Cocoa", "IOKit", "CoreFoundation"])
