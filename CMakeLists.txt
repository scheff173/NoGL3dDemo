project(qNoGL3dDemo)

file(GLOB headers *.h)
file(GLOB sources *.cc)

#find_package(Qt5Widgets CONFIG REQUIRED)

add_executable(qNoGL3dDemo
  ${sources} ${headers})
  
target_link_libraries(qNoGL3dDemo
  Qt5::Widgets)
