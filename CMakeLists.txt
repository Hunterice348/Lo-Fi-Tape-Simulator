cmake_minimum_required(VERSION 3.15)
project(LoFiTapeSimulator VERSION 1.0.0)

# Подключаем JUCE (папка JUCE должна лежать рядом с этим файлом при сборке)
add_subdirectory(JUCE)

# Настраиваем плагин
juce_add_plugin(LoFiTapeSimulator
    COMPANY_NAME "LoFiLabs"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    PLUGIN_MANUFACTURER_CODE Lofi
    PLUGIN_CODE Tape
    FORMATS VST3
    PRODUCT_NAME "Lo-Fi Tape Simulator"
)

# Указываем исходные файлы
target_sources(LoFiTapeSimulator PRIVATE
    PluginProcessor.cpp
    PluginProcessor.h
    PluginEditor.cpp
    PluginEditor.h
)

# Настройки компиляции
target_compile_features(LoFiTapeSimulator PRIVATE cxx_std_17)
juce_generate_juce_header(LoFiTapeSimulator)
