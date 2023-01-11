
function(run_inja_template DATA TEMPLATE OUTPUT)
    # Add custom command to run C preprocessor.
    #
    # Arguments
    #   DATA            input file
    #   TEMPLATE        jinja template
    #   OUTPUT          output file

    add_custom_command(
        OUTPUT ${OUTPUT}
        COMMAND inja
            --data ${DATA}
            --template ${TEMPLATE}
            --output ${OUTPUT}
        DEPENDS ${DATA} ${TEMPLATE}
        COMMENT "Generating ${OUTPUT} from ${DATA} using jinja template ${TEMPLATE}"
    )
endfunction()
