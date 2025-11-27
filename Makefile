# Makefile
# La regla 'all' en el makefile principal
all: build_all

# Una regla para construir todo en la carpeta 'build'
build_all:
	# Cambia al directorio 'build' y ejecuta la regla 'all' de su Makefile
	$(MAKE) -C build

# Regla de limpieza que también llama a la limpieza en el subdirectorio
clean:
	@echo "Limpiando en el subdirectorio 'build'..."
	$(MAKE) -C build clean
	@echo "Limpieza del directorio raíz completada."