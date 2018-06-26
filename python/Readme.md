Python bindings are provided by `pybind11`. Set it up as a git submodule using `git submodule init; git submodule update`.
(Note that it is only tested for python 2.7, patch:
```
diff --git a/tools/FindPythonLibsNew.cmake b/tools/FindPythonLibsNew.cmake
index b29b287..c967066 100644
--- a/tools/FindPythonLibsNew.cmake
+++ b/tools/FindPythonLibsNew.cmake
@@ -57,9 +57,9 @@ endif()
 
 # Use the Python interpreter to find the libs.
 if(PythonLibsNew_FIND_REQUIRED)
-    find_package(PythonInterp ${PythonLibsNew_FIND_VERSION} REQUIRED)
+    find_package(PythonInterp 2.7 REQUIRED)
 else()
-    find_package(PythonInterp ${PythonLibsNew_FIND_VERSION})
+    find_package(PythonInterp 2.7)
 endif()
 
```





