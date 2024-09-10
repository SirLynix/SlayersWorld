add_rules("mode.debug", "mode.release")
add_requires("libcurl", "sfml", "sqlite3")

set_exceptions("cxx")
set_encodings("utf-8")
set_runtimes(is_mode("debug") and "MDd" or "MD")
set_rundir(".")
set_languages("cxx14")

target("Client", function ()
    add_headerfiles("Client/client/**.hpp")
    add_files("Client/client/**.cpp")
    if is_plat("windows") then
        add_files("Client/client/resource.rc")
    end

    set_basename("SlayersWorld")

    add_packages("sfml", { components = {"system", "window", "graphics", "network"} })
end)

target("Server", function ()
    add_headerfiles("Server/server/**.h")
    add_headerfiles("Server/server/**.hpp")
    add_files("Server/server/**.cpp")

    set_basename("SlayersServer")

    add_packages("libcurl", "sqlite3")
    add_packages("sfml", { components = {"system", "network"} })
end)