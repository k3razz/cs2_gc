
if(DEFINED OUTDIR)
    install(TARGETS Injector GC_Core
        RUNTIME DESTINATION ${OUTDIR}
        LIBRARY DESTINATION ${OUTDIR}
    )
endif()

if(DEFINED OUTDIR)
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json 
         DESTINATION ${OUTDIR})
    file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/resources/schema_2026.json 
         DESTINATION ${OUTDIR})
endif()
