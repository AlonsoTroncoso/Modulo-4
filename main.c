#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_COMPRAS 50
#define MAX_VENTAS 50

int contadorClientes = 0;

struct MercaditoLibre
{
    struct NodoProveedores *headProveedores;
    struct NodoClientes *headClientes;
    struct listaProductos *productos;
    struct NodoBodegas *headBodegas;
    struct Inventario *datosInventario;
};

struct listaProductos
{
    int cantProductos;
    struct NodoProductos *abbProducto;
};

struct NodoProductos
{
    struct Productos *datosProducto;
    struct NodoProductos *izq, *der;
};

struct NodoProveedores
{
    struct Proveedores *datosProveedores;
    struct NodoProveedores *sig;

};

struct NodoClientes
{
    struct Cliente *datosCliente;
    struct NodoClientes *sig;

};

struct NodoBodegas
{
    struct Bodega *datosBodega;
    struct NodoBodegas *sig;

};

struct Compra
{
    int codigoBoleta;
    int precioProducto;
    int idProducto;
    int cantProducto;
    char nombre[50];
    char *fecha;
    char **productosComprados;

};

struct Proveedores
{
    int id;
    int vendido;
    int numProductos;
    char *nombre;
    struct Compra ventasAlaEmpresa[MAX_VENTAS];

};

struct Cliente
{
    int id;
    int registro;
    int cont;
    char *nombre;
    int gastado;
    int numProductos;
    struct Compra compras[MAX_COMPRAS];
};

struct Bodega{
    int id;
    int capacidadProductos;
    char *direccion;
    struct Inventario **stockDeBodega;

};

//el struct inventario tiene un id ya que use el mismo struct tanto para el stock general//
//a la vez que para el de cada bodega por separado asi podremos diferenciarlos //

struct Inventario
{
    int id;
    struct NodoProductos **headProductos;

};

struct Productos
{
    int id;
    char *nombre;
    int precio;
    int stock;
    int movimientos;
    struct Flujo *flujoProducto;

};

//use un struct para el flujo asi podemos llevar la cant de compras y ventas de cada producto//
//a la vez que el flujo total del producto//

struct Flujo
{
    int cantComprada;
    int cantVendida;
    int comprasTotales;
};


//aqui van las funciones//

void convertirAMayusculas(char *cadena)
{
    int i = 0;
    while (cadena[i])
    {
        cadena[i] = toupper(cadena[i]);
        i++;
    }
}

struct NodoClientes *obtenerPunteroMedio(struct NodoClientes *inicio, struct NodoClientes *fin)
{
    struct NodoClientes *liebre = inicio;
    struct NodoClientes *tortuga = inicio;

    while (liebre != fin && liebre->sig != fin)
    {
        //hago esto porque cuando la liebre llegue al final la tortuga estara en la mitad
        liebre = liebre->sig->sig;
        tortuga = tortuga->sig;
    }

    return tortuga;
}

int comprobarCliente(struct MercaditoLibre *mercado, int idIngresado)
{
    struct NodoClientes *inicio = mercado->headClientes;
    struct NodoClientes *fin = NULL;

    if(inicio==NULL) return 0;

    while (inicio != NULL && inicio != fin)
    {
        struct NodoClientes *medio = obtenerPunteroMedio(inicio, fin);

        if (medio->datosCliente->id == idIngresado)
        {
            return 1; // existe el cliente
        }
        else if (medio->datosCliente->id < idIngresado)
        {
            inicio = medio->sig;
        }
        else
        {
            fin = medio;
        }
    }

    return 0; // no existe el cliente
}

int Comprobarproveedor (struct MercaditoLibre *mercado, int idIngresado)
{
    struct NodoProveedores *rec = mercado->headProveedores;
    while (rec!=NULL)
    {
        if (rec->datosProveedores->id == idIngresado) return 1; //existe el proveedor
        rec = rec->sig;
    }
    return 0; //no existe el proveedor
}

void asignarDatosCliente (struct Cliente *nuevo, char *nombre)
{
    nuevo->nombre=(char *)malloc(strlen(nombre)*sizeof(char));
    if (nuevo->nombre == NULL) {
        printf("Error: No se pudo asignar memoria para el nombre del cliente.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(nuevo->nombre,nombre);
    nuevo->gastado=0;
    nuevo->numProductos=0;
    nuevo->registro = ++contadorClientes;
}

struct Cliente *crearCliente(struct MercaditoLibre *mercado)
{
    struct Cliente *nuevo;
    char nombre[50];
    int resultado;
    nuevo=(struct Cliente *)malloc(sizeof(struct Cliente));

    if (nuevo == NULL)
    {
        printf("Error: No se pudo asignar memoria para el cliente.\n");
        exit(EXIT_FAILURE);
    }

    do
    {
        printf("Ingrese el ID del cliente: ");
        resultado = scanf("%d", &(nuevo->id));

        if (resultado != 1)
        {
            printf("Error: Ingresa un ID valido (numero entero).\n\n");
            while (getchar() != '\n');
        }
    } while (resultado != 1);

    if(comprobarCliente(mercado,nuevo->id)==1)
    {
        return NULL;
    }


    printf("Ingrese nombre del cliente: ");
    scanf("%s",nombre);

    asignarDatosCliente(nuevo,nombre);



    return nuevo;
}

struct NodoClientes *crearNodo (struct Cliente *nueva)
{
    struct NodoClientes *nuevo=NULL;
    if(nueva != NULL)
    {
        nuevo=(struct NodoClientes *)malloc(sizeof(struct NodoClientes));
        if (nuevo == NULL) {
            printf("Error: No se pudo asignar memoria para el nodo del cliente.\n");
            exit(EXIT_FAILURE);
        }
        nuevo->datosCliente=nueva;
        nuevo->sig=NULL;
    }

    return nuevo;
}

void agregarCliente(struct MercaditoLibre *mercado,struct NodoClientes **head)
{
    struct NodoClientes *nuevo=NULL;
    struct NodoClientes *rec=NULL;
    nuevo=crearNodo(crearCliente(mercado));

    if(nuevo == NULL)
    {
        printf("El cliente ya existe.\n\n");
        return;
    }

    // Caso especial: Lista vacía o el nuevo cliente tiene un ID menor que el primer cliente
    if (*head == NULL || nuevo->datosCliente->id < (*head)->datosCliente->id)
    {
        nuevo->sig = *head;
        *head = nuevo;
        printf("Cliente agregado con exito.\n\n");
        return;
    }

    // Buscar la posición de inserción
    struct NodoClientes *actual = *head;
    struct NodoClientes *anterior = NULL;
    while (actual != NULL && nuevo->datosCliente->id > actual->datosCliente->id)
    {
        anterior = actual;
        actual = actual->sig;
    }
    // Insertar el nuevo nodo en la posición adecuada
    nuevo->sig = actual;
    anterior->sig = nuevo;
    printf("Cliente agregado con exito.\n\n");

}

void asignarDatosProveedor (struct Proveedores *nuevo, char *buffer)
{
    nuevo->nombre=(char *)malloc(strlen(buffer)*sizeof(char));
    if (nuevo->nombre == NULL) {
        printf("Error: No se pudo asignar memoria para el nombre del proveedor.\n");
        exit(EXIT_FAILURE);
    }
    strcpy(nuevo->nombre,buffer);
    nuevo->vendido=0;
    nuevo->numProductos=0;

}

struct Proveedores *crearProveedor(struct MercaditoLibre *mercado)
{
    struct Proveedores *nuevo;
    char buffer[50];
    int resultado;
    nuevo=(struct Proveedores *)malloc(sizeof(struct Proveedores));

    if (nuevo == NULL)
    {
        printf("Error: No se pudo asignar memoria para el proveedor.\n");
        exit(EXIT_FAILURE);
    }

    do
    {
        printf("Ingrese el ID del proveedor: ");
        resultado = scanf("%d", &(nuevo->id));

        if (resultado != 1)
        {
            printf("Error: Ingresa un ID valido (numero entero).\n\n");
            while (getchar() != '\n');
        }
    } while (resultado != 1);

    if(Comprobarproveedor(mercado,nuevo->id)==1)
    {
        return NULL;
    }


    printf("Ingrese nombre del proveedor: ");
    scanf("%s",buffer);

    asignarDatosProveedor(nuevo,buffer);

    return nuevo;
}

struct NodoProveedores *crearNodoP (struct Proveedores *nueva)
{
    struct NodoProveedores *nuevo=NULL;
    if(nueva != NULL)
    {
        nuevo=(struct NodoProveedores *)malloc(sizeof(struct NodoProveedores));
        if (nuevo == NULL) {
            printf("Error: No se pudo asignar memoria para el nodo del proveedor.\n");
            exit(EXIT_FAILURE);
        }
        nuevo->datosProveedores=nueva;
        nuevo->sig=NULL;
    }

    return nuevo;
}

void agregarProveedor (struct MercaditoLibre *mercado,struct NodoProveedores **head)
{
    struct NodoProveedores *nuevo=NULL;
    struct NodoProveedores *rec=NULL;
    nuevo=crearNodoP(crearProveedor(mercado));

    if(nuevo==NULL)
    {
        printf("El proveedor ya existe.\n");
        return;
    }

    if(*head == NULL)
    {
        *head=nuevo;
        printf("Proveedor agregado con exito.\n\n");
        return;
    }

    else
    {
        rec=*head;
        while(rec->sig!=NULL)
        {
            rec=rec->sig;
        }

        rec->sig=nuevo;
        printf("Proveedor agregado con exito.\n\n");
        return;
    }
}

void procesoEliminarCliente (struct MercaditoLibre *mercado, int idIngresado)
{
    struct NodoClientes *rec = mercado->headClientes;

    if (comprobarCliente(mercado,idIngresado) == 0) //uso la función ya hecha para comprobar si el cliente NO se encuetra
    {
        printf("El cliente no se encuentra registrado.\n\n");//En caso de que el cliente no exista
        return;
    }

    if(rec->datosCliente == NULL) return; //en caso de que no existan datos del cliente

    if (rec->datosCliente->id == idIngresado) //si es el head el cliente a eliminar
    {
        mercado->headClientes = rec->sig;
        printf("el cliente fue quitado con exito\n\n");
        return;
    }

    while(rec->sig != NULL)
    {
        if (rec->sig->datosCliente->id == idIngresado)
        {
            rec->sig= rec->sig->sig;
            printf("el cliente fue quitado con exito\n\n");
            return;
        }
        rec=rec->sig;
    }
}

void eliminarCliente (struct MercaditoLibre *mercado)
{
    int idIngresado;

    if (mercado->headClientes == NULL)
    {
        printf("La lista de clientes esta vacia. No hay clientes para eliminar.\n\n");
        return;
    }
    else
    {
        printf("\nIngrese el id del cliente que desea quitar: ");
        scanf("%d", &idIngresado);
        procesoEliminarCliente(mercado,idIngresado);
        return;
    }

}

void procesoEliminarProv (struct MercaditoLibre *mercado, int idIngresado)
{
    struct NodoProveedores *rec = mercado->headProveedores;

    if (Comprobarproveedor(mercado,idIngresado) == 0) //uso la función ya hecha para comprobar si el cliente NO se encuetra
    {
        printf("El proveedor no se encuentra registrado.\n\n");//En caso de que el cliente no exista
        return;
    }

    if(rec->datosProveedores == NULL) return; //en caso de que no existan datos del cliente

    if (rec->datosProveedores->id == idIngresado) //si es el head el cliente a eliminar
    {
        mercado->headProveedores = rec->sig;
        printf("el proveedor fue quitado con exito\n\n");
        return;
    }

    while(rec->sig != NULL)
    {
        if (rec->sig->datosProveedores->id == idIngresado)
        {
            rec->sig= rec->sig->sig;
            printf("el proveedor fue quitado con exito\n\n");
            return;
        }
        rec=rec->sig;
    }

}

void eliminarProveedor (struct MercaditoLibre *mercado)
{
    int idIngresado;

    if (mercado->headProveedores == NULL)
    {
        printf("La lista de proveedores esta vacia. No hay proveedores para eliminar.\n\n");
        return;
    }
    else
    {
        printf("\nIngrese el id del proveedor que desea quitar: ");
        scanf("%d", &idIngresado);
        procesoEliminarProv(mercado,idIngresado);
        return;
    }

}

void cambioNombreC (struct Cliente *clienteEncontrado, char *nuevoNombre,int idIngresado)
{
    clienteEncontrado->nombre = (char*)malloc(strlen(nuevoNombre) + 1); // Asignar memoria para el nuevo nombre
    if (clienteEncontrado->nombre != NULL)
    {
        strcpy(clienteEncontrado->nombre, nuevoNombre);
        printf("Cliente con ID %d modificado correctamente.\n\n", idIngresado);
        return;
    }
    else
    {
        printf("Error: No se pudo asignar memoria para el nombre del producto.\n\n");
        return;
    }
}

void cambioNombreP (struct Proveedores *clienteEncontrado, char *nuevoNombre,int idIngresado)
{
    clienteEncontrado->nombre = (char*)malloc(strlen(nuevoNombre) + 1); // Asignar memoria para el nuevo nombre
    if (clienteEncontrado->nombre != NULL)
    {
        strcpy(clienteEncontrado->nombre, nuevoNombre);
        printf("Proveedor con ID %d modificado correctamente.\n\n", idIngresado);
        return;
    }
    else
    {
        printf("Error: No se pudo asignar memoria para el nombre del producto.\n\n");
        return;
    }
}

void modificarCliente (struct MercaditoLibre *mercado)
{
    int idIngresado;
    char nuevoNombre[50];
    struct NodoClientes *rec = mercado->headClientes;
    struct Cliente *clienteEncontrado = NULL;

    if (rec == NULL)
    {
        printf("La lista de clientes esta vacia. No hay clientes para modificar.\n\n");
        return;
    }
    else
    {
        printf("\nIngrese el id del cliente que desea modificar: ");
        scanf("%d", &idIngresado);
    }

    if (comprobarCliente(mercado,idIngresado)==0)
    {
        printf("El cliente no existe.\n\n");
        return;
    }

    while (rec != NULL)
    {
        if (rec->datosCliente->id == idIngresado)
        {
            clienteEncontrado = rec->datosCliente;
            convertirAMayusculas(clienteEncontrado->nombre);
            printf("    ## CLIENTE ENCONTRADO ##    \n");
            printf(" -------------------------------\n");
            printf("|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID");
            printf(" -------------------------------\n");
            printf("|%-*s|%-*d|\n", -15, clienteEncontrado->nombre, -15, clienteEncontrado->id);
            printf(" -------------------------------\n");
            printf("\n");

            printf("Ingrse el nuevo nombre: ");
            scanf("%s",nuevoNombre);

            cambioNombreC(clienteEncontrado,nuevoNombre,idIngresado);
        }
        rec = rec->sig;
    }

    if (clienteEncontrado == NULL)
    {
        printf("El cliente no se encuentra registrado.\n\n");
        return;
    }

}

void modificarProveedor (struct MercaditoLibre *mercado)
{
    int idIngresado;
    char nuevoNombre[50];
    struct NodoProveedores *rec = mercado->headProveedores;
    struct  Proveedores *proveedorEncontrado = NULL;

    if (rec == NULL)
    {
        printf("La lista de proveedores esta vacia. No hay proveedores para modificar.\n\n");
        return;
    }
    else
    {
        printf("\nIngrese el id del proveedor que desea modificar: ");
        scanf("%d", &idIngresado);
    }

    if (Comprobarproveedor(mercado,idIngresado)==0)
    {
        printf("El proveedor no existe.\n\n");
        return;
    }

    while (rec != NULL)
    {
        if (rec->datosProveedores->id == idIngresado)
        {
            proveedorEncontrado = rec->datosProveedores;
            convertirAMayusculas(proveedorEncontrado->nombre);
            printf("   ## PROVEEDOR ENCONTRADO ##  \n");
            printf(" -------------------------------\n");
            printf("|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID");
            printf(" -------------------------------\n");
            printf("|%-*s|%-*d|\n", -15, proveedorEncontrado->nombre, -15, proveedorEncontrado->id);
            printf(" -------------------------------\n");
            printf("\n");

            printf("Ingrse el nuevo nombre: ");
            scanf("%s",nuevoNombre);

            cambioNombreP(proveedorEncontrado,nuevoNombre,idIngresado);
        }
        rec = rec->sig;
    }

    if (proveedorEncontrado == NULL)
    {
        printf("El proveedor no se encuentra registrado.\n\n");
        return;
    }

}

void cambioNombreProducto(struct NodoProductos* nodo, char *nuevoNombre,int idProducto)
{
    nodo->datosProducto->nombre = (char*)malloc(strlen(nuevoNombre) + 1); // Asignar memoria para el nuevo nombre
    if (nodo->datosProducto->nombre != NULL)
    {
        strcpy(nodo->datosProducto->nombre, nuevoNombre);
        printf("Producto con ID %d modificado correctamente.\n\n", idProducto);
    }
    else
    {
        printf("Error: No se pudo asignar memoria para el nombre del producto.\n\n");
    }
}

void cambioProductoPrecio(struct NodoProductos* nodo, int nuevoPrecio,int idProducto)
{
    nodo->datosProducto->precio = nuevoPrecio;
    printf("Producto con ID %d modificado correctamente.\n\n", idProducto);
}

void cambioProductoStock(struct NodoProductos* nodo, int nuevoStock,int idProducto)
{
    nodo->datosProducto->stock = nuevoStock;
    printf("Producto con ID %d modificado correctamente.\n\n", idProducto);
}

void modificarProducto(struct NodoProductos* nodo, int idProducto)
{
    char nuevoNombre[50];
    int nuevoPrecio,nuevoStock,opcion;

    if (nodo == NULL)
    {
        printf("Producto con ID %d no encontrado. No se puede modificar.\n\n", idProducto);
        return;
    }

    if (idProducto < nodo->datosProducto->id)
    {
        modificarProducto(nodo->izq, idProducto);
    }
    else if (idProducto > nodo->datosProducto->id)
    {
        modificarProducto(nodo->der, idProducto);
    }
    else
    {
        // Modificar el precio y el nombre del producto
        printf("\n                 ## PRODUCTO ENCONTRADO ##\n");
        printf(" ----------------------------------------------------------------\n");
        printf("|%-*s|%-*s|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID",+ 15, "Precio",+ 15, "Stock");
        printf(" ----------------------------------------------------------------\n");
        printf("|%-*s|%-*d|%-*d|%-*d|\n", -15, nodo->datosProducto->nombre, -15, nodo->datosProducto->id, -15, nodo->datosProducto->precio, -15, nodo->datosProducto->stock);
        printf(" ----------------------------------------------------------------\n\n");

        printf(" ------------------------ \n");
        printf("|Que desea modificar     |\n");
        printf("|------------------------|\n");
        printf("|1 Nombre                |\n");
        printf("|------------------------|\n");
        printf("|2 Precio                |\n");
        printf("|------------------------|\n");
        printf("|3 Stock                 |\n");
        printf(" ------------------------\n\n");
        printf("Ingrese su respuesta: ");
        scanf("%d", &opcion);
        printf("\n");

        if(opcion == 1)
        {
            printf("Ingrse el nuevo nombre: ");
            scanf("%s",nuevoNombre);
            cambioNombreProducto(nodo,nuevoNombre,idProducto);

        }

        else if (opcion == 2)
        {
            printf("Ingrese nuevo precio: ");
            scanf("%d",&nuevoPrecio);
            cambioProductoPrecio(nodo,nuevoPrecio,idProducto);
        }

        else if (opcion == 3)
        {
            printf("Ingrese nuevo stock: ");
            scanf("%d",&nuevoStock);
            cambioProductoStock(nodo,nuevoStock,idProducto);
        }
        else
        {
            printf("Error: opcion invalida.\n\n");
            return;
        }
    }
}

void agregarCant (struct NodoProductos *abb, int idIngresado)
{

    if (abb != NULL && abb->datosProducto != NULL)
    {
        int cantidad=0;

        agregarCant(abb->izq, idIngresado);
        if (abb->datosProducto->id == idIngresado)
        {
            printf("\nProducto con ID %d encontrado.\n",idIngresado);
            printf("Ingrese la cantidad que desea agregar: ");
            scanf("%d",&cantidad);
            abb->datosProducto->stock += cantidad;
            abb->datosProducto->movimientos++;
            printf("\nCantidad del producto con ID %d y nombre %s incrementada a %d.\n\n", idIngresado,abb->datosProducto->nombre, abb->datosProducto->stock);
            return;
        }
        agregarCant(abb->der, idIngresado);
    }
    else
    {
        return;
    }
}

void mostrarClientes(struct NodoClientes *headClientes)
{
    if (headClientes==NULL)
    {
        printf("\nNo hay clientes para mostrar.\n\n");
        return;
    }
    printf("   ## LISTA DE CLIENTES ##\n");
    printf(" -------------------------------\n");
    printf("|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID");
    printf(" -------------------------------\n");
    struct NodoClientes *rec = headClientes;
    while(rec != NULL)
    {
        printf("|%-*s|%-*d|\n", -15, rec->datosCliente->nombre, -15, rec->datosCliente->id);
        rec = rec->sig;
    }
    printf(" -------------------------------\n");
    printf("\n");
}

void mostrarProveedores(struct NodoProveedores *headProveedores)
{
    if(headProveedores == NULL)
    {
        printf("\nNo hay proveedores para mostrar.\n\n");
        return;
    }
    printf("   ## LISTA DE PROVEEDORES ##\n");
    printf(" -------------------------------\n");
    printf("|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID");
    printf(" -------------------------------\n");
    struct NodoProveedores *rec = headProveedores;
    while(rec != NULL)
    {
        printf("|%-*s|%-*d|\n", -15, rec->datosProveedores->nombre, -15, rec->datosProveedores->id);
        rec = rec->sig;
    }
    printf(" -------------------------------\n");
    printf("\n");
}

void mostrarComprasClientes(struct MercaditoLibre *mercado)
{
    int idCliente;
    struct NodoClientes *rec = mercado->headClientes;

    if (rec == NULL)
    {
        printf("La lista de clientes esta vacia.\n\n");
        return;
    }
    else
    {
        printf("Ingrese el id del cliente: ");
        scanf("%d",&idCliente);
    }

    while (rec != NULL)
    {
        if(rec->datosCliente->id == idCliente)
        {
            printf("Cliente ID: %d\n", rec->datosCliente->id);
            printf("Nombre cliente: %s\n\n",rec->datosCliente->nombre);
            if (rec->datosCliente->numProductos > 0)
            {
                printf("                    ## LISTA DE PRODUCTOS ##\n");
                printf(" ----------------------------------------------------------------\n");
                printf("|%-*s|%-*s|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID",+ 15, "Precio",+ 15, "Cantidad ");
                printf(" ----------------------------------------------------------------\n");
                for (int i = 0; i < rec->datosCliente->numProductos; i++)
                {
                    printf("|%-*s|%-*d|%-*d|%-*d|\n", -15, rec->datosCliente->compras[i].nombre, -15, rec->datosCliente->compras[i].idProducto, -15, rec->datosCliente->compras[i].precioProducto, -15, rec->datosCliente->compras[i].cantProducto);
                }
                printf(" ----------------------------------------------------------------\n");
                printf("\n");
                printf(" --------------------------------------------------\n");
                printf("| MONTO TOTAL GASTADO DEL CLIENTE: %-15d |\n", rec->datosCliente->gastado);
                printf(" --------------------------------------------------\n\n");
                return;
            }
            else
            {
                printf("El cliente no ha realizado compras.\n\n");
                return;
            }
        }
        rec = rec->sig;
    }

    printf("El cliente no esta registrado.\n\n");
    return;

}

void mostrarProductosRecursivo (struct NodoProductos *abb)
{
    if (abb==NULL)
    {
        return;
    }
    mostrarProductosRecursivo(abb->izq);
    // Luego mostramos el producto en este nodo
    printf("|%-*s|%-*d|%-*d|%-*d|\n", -15, abb->datosProducto->nombre, -15, abb->datosProducto->id, -15, abb->datosProducto->precio, -15, abb->datosProducto->stock);
    // Finalmente, recorremos el subárbol derecho
    mostrarProductosRecursivo(abb->der);
}

void mostrarProductos(struct NodoProductos *abb)
{
    if (abb==NULL)
    {
        printf("NO hay productos disponibles.\n\n");
        return;
    }

    // Encabezado del cuadrado
    printf("                    ## LISTA DE PRODUCTOS ##\n");
    printf(" ----------------------------------------------------------------\n");
    printf("|%-*s|%-*s|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID",+ 15, "Precio",+ 15, "Stock");
    printf(" ----------------------------------------------------------------\n");

    // Iterar sobre los productos y mostrar los detalles en un cuadrado simétrico
    mostrarProductosRecursivo(abb);
    printf(" ----------------------------------------------------------------\n");
    printf("\n");

}

int calcularTotalGastado(struct MercaditoLibre *mercado)
{
    int totalGastado = 0;
    struct NodoClientes *rec = mercado->headClientes;

    while (rec != NULL) {
        totalGastado += rec->datosCliente->gastado;
        rec = rec->sig;
    }

    return totalGastado;
}

struct Productos *buscarProductoConMenorRotacion(struct NodoProductos *abb, struct Productos *productoMenor)
{
    if (abb == NULL)
    {
        return productoMenor;
    }

    if (abb->datosProducto != NULL && (productoMenor == NULL || abb->datosProducto->movimientos < productoMenor->movimientos))
    {
        productoMenor = abb->datosProducto;
    }

    struct Productos *productoIzq = buscarProductoConMenorRotacion(abb->izq, productoMenor);
    struct Productos *productoDer = buscarProductoConMenorRotacion(abb->der, productoMenor);

    // Comparamos el producto encontrado en el subárbol izquierdo y derecho
    if (productoIzq != NULL && (productoDer == NULL || productoIzq->movimientos < productoDer->movimientos))
    {
        return productoIzq;
    }
    else
    {
        return productoDer;
    }
}

struct Productos *obtenerProductoMenorRotacion(struct NodoProductos *abb)
{
    if (abb == NULL)
    {
        return NULL;
    }

    return buscarProductoConMenorRotacion(abb, NULL);
}

struct Productos *buscarProductoConMayorRotacion(struct NodoProductos *abb, struct Productos *productoMayor)
{
    if (abb == NULL)
    {
        return productoMayor;
    }

    if (abb->datosProducto != NULL && (productoMayor == NULL || abb->datosProducto->movimientos > productoMayor->movimientos))
    {
        productoMayor = abb->datosProducto;
    }

    struct Productos *productoIzq = buscarProductoConMayorRotacion(abb->izq, productoMayor);
    struct Productos *productoDer = buscarProductoConMayorRotacion(abb->der, productoMayor);

    // Comparamos el producto encontrado en el subárbol izquierdo y derecho
    if (productoIzq != NULL && (productoDer == NULL || productoIzq->movimientos > productoDer->movimientos))
    {
        return productoIzq;
    }
    else
    {
        return productoDer;
    }
}

struct Productos *obtenerProductoMayorRotacion(struct NodoProductos *abb)
{
    if (abb == NULL)
    {
        return NULL;
    }

    return buscarProductoConMayorRotacion(abb, NULL);

}

struct NodoProductos* buscarProducto (struct NodoProductos *abb, int id)
{
    if (abb == NULL || abb->datosProducto == NULL) return NULL;

    if (id < abb->datosProducto->id)
    {
        return buscarProducto(abb->izq, id);
    }
    else if (id > abb->datosProducto->id)
    {
        return buscarProducto(abb->der, id);
    }
    else
    {
        return abb;
    }
}

void modificarProductoabb(struct MercaditoLibre* mercado)
{
    int idProducto;

    if(mercado->productos->abbProducto == NULL)
    {
        printf("No hay productos registrados para modificar.\n\n");
        return;
    }
    else
    {
        printf("\nIngrese el id del producto que desea modificar: ");
        scanf("%d", &idProducto);
    }
    if(buscarProducto(mercado->productos->abbProducto,idProducto)==NULL)
    {
        printf("El producto ingresado no existe.\n\n");
        return;
    }
    modificarProducto(mercado->productos->abbProducto, idProducto);
}

void agregarCarrito(struct MercaditoLibre *mercado, int idCliente, int idProducto, int cant)
{
    struct NodoClientes *rec = mercado->headClientes;

    if (rec == NULL)
    {
        printf("La lista de clientes esta vacia.\n\n");
        return;
    }

    while (rec != NULL) {
        if (rec->datosCliente->id == idCliente)
        {
            struct NodoProductos *productoEncontrado = buscarProducto(mercado->productos->abbProducto, idProducto);

            if (productoEncontrado != NULL)
            {

                if (rec->datosCliente->numProductos < MAX_COMPRAS)
                {
                    int indice = rec->datosCliente->numProductos;

                    rec->datosCliente->compras[indice].idProducto = idProducto;
                    rec->datosCliente->compras[indice].precioProducto = productoEncontrado->datosProducto->precio;
                    rec->datosCliente->compras[indice].cantProducto=cant;

                    strcpy(rec->datosCliente->compras[indice].nombre, productoEncontrado->datosProducto->nombre);

                    rec->datosCliente->gastado += (productoEncontrado->datosProducto->precio * cant);
                    rec->datosCliente->numProductos++;

                    printf("Producto agregado al carrito correctamente.\n");
                    printf("TOTAL GASTADO POR CLIENTE = %d\n\n", rec->datosCliente->gastado);
                    return;
                }
                else
                {
                    printf("Error: El carrito del cliente esta lleno. No se pueden agregar mas productos.\n\n");
                    return;
                }
            }
            else
            {
                printf("Producto no encontrado.\n\n");
                return;
            }
        }
        rec = rec->sig;
    }

    printf("Cliente no encontrado.\n\n");
    return;
}

void venderProducto(struct MercaditoLibre *mercado)
{
    int producto,idIngresado, cant = 0;
    struct NodoProductos *recP = mercado->productos->abbProducto;
    struct NodoClientes *recC = mercado->headClientes;
    struct Cliente *clienteEncontrado = NULL;

    if (recC==NULL)
    {
        printf("La lista de clientes esta vacia. No hay clientes para vender.\n\n");
        return;
    }

    else if(recP==NULL)
    {
        printf("La lista de productos esta vacia. No hay productos para vender.\n\n");
        return;
    }

    else
    {
        printf("Ingrese el ID del cliente: ");
        scanf("%d",&idIngresado);
    }

    while (recC != NULL)
    {
        if (recC->datosCliente->id == idIngresado)
        {
            clienteEncontrado = recC->datosCliente;
            break;
        }
        recC = recC->sig;
    }

    if (clienteEncontrado == NULL)
    {
        printf("El cliente no se encuentra registrado.\n\n");
        return;
    }
    printf("## CLIENTE ENCONTRADO ##\n");
    convertirAMayusculas(clienteEncontrado->nombre);
    printf("NOMBRE CLIENTE : %s\n\n",clienteEncontrado->nombre);

    printf("Ingrese el ID del producto: ");
    scanf("%d", &producto);
    printf("\n");

    struct NodoProductos *productoEncontrado = buscarProducto(recP, producto);

    if (productoEncontrado != NULL)
    {
        if(productoEncontrado->datosProducto->stock == 0)
        {
            printf("\n        PRODUCTO AGOTADO           \n");
            printf("CONSULTE SOBRE LLEGADA DE NUEVO STOCK\n");
            printf("     TELEFONO: +56947901390\n\n");
            return;
        }
        printf("                    ## PRODUCTO ENCONTRADO ##\n");
        printf(" ----------------------------------------------------------------\n");
        printf("|%-*s|%-*s|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID",+ 15, "Precio",+ 15, "Stock");
        printf(" ----------------------------------------------------------------\n");
        printf("|%-*s|%-*d|%-*d|%-*d|\n", -15, productoEncontrado->datosProducto->nombre, -15, productoEncontrado->datosProducto->id, -15, productoEncontrado->datosProducto->precio, -15, productoEncontrado->datosProducto->stock);
        printf(" ----------------------------------------------------------------\n\n");

        do
        {
            printf("Ingrese la cantidad que desea comprar: ");
            scanf("%d", &cant);
            if(cant==0)
            {
                printf("No se ha vendido ningun producto\n\n");
                return;
            }
            if (cant > productoEncontrado->datosProducto->stock || cant <= 0)
            {
                printf("La cantidad a comprar no es valida o supera el stock disponible.\n");
            }

        } while (cant > productoEncontrado->datosProducto->stock || cant < 0);

        productoEncontrado->datosProducto->stock -= cant;
        productoEncontrado->datosProducto->movimientos++;
        if(productoEncontrado->datosProducto->stock==0)
        {
            printf("\n\n\n                   !! PRECAUCION !!            \n");
            printf("            PRODUCTO AGOTADO PARA VENTAS FUTURAS \n");
            printf("POR FAVOR CONTACTARSE CON UN PROVEEDOR PARA REABASTECER EL STOCK\n\n\n");
        }

        if(productoEncontrado->datosProducto->stock<=10 && productoEncontrado->datosProducto->stock >=1)
        {
            printf("\n\n\n                      !! PRECAUCION !!            \n");
            printf("                   QUEDAN POCAS UNIDADES  \n");
            printf("POR FAVOR CONTACTARSE CON UN PROVEEDOR PARA REABASTECER EL STOCK\n\n\n");
        }

        if (recC->datosCliente != NULL)
        {
            agregarCarrito (mercado, idIngresado ,productoEncontrado->datosProducto->id,cant);
            return;
        }
        else
        {
            printf("Error: No existe el cliente.\n\n");
        }
    }
    else
    {
        printf("Producto no encontrado.\n\n");
    }
}

struct NodoProductos * insertarProductos(struct NodoProductos* abb, struct Productos* nuevoProducto)
{
    if (abb == NULL)
    {
        struct NodoProductos* nuevoNodo = (struct NodoProductos*)malloc(sizeof(struct NodoProductos));
        nuevoNodo->datosProducto = nuevoProducto;
        nuevoNodo->izq = NULL;
        nuevoNodo->der = NULL;
        return nuevoNodo;
    }

    if (nuevoProducto->id < abb->datosProducto->id)
    {
        abb->izq = insertarProductos(abb->izq, nuevoProducto);
    }
    else if (nuevoProducto->id > abb->datosProducto->id)
    {
        abb->der = insertarProductos(abb->der, nuevoProducto);
    }

    return abb;
}

void datosProducto (struct Productos *nuevoProducto)
{
    printf("Ingrese el nombre del producto: ");
    scanf("%s", nuevoProducto->nombre);
    printf("Ingrese el precio del producto: ");
    scanf("%d",&(nuevoProducto->precio));
    printf("Ingrese el stock del producto: ");
    scanf("%d",&(nuevoProducto->stock));
    nuevoProducto->movimientos=1;

}

void agregarProducto(struct MercaditoLibre *mercado)
{
    struct Productos *nuevoProducto = (struct Productos*)malloc(sizeof(struct Productos));
    struct NodoProveedores *recP=mercado->headProveedores;
    struct Proveedores *proveedorEncontrado = NULL;
    nuevoProducto->nombre = (char*)malloc(30 * sizeof(char));

    int prov=0;
    if (nuevoProducto == NULL)
    {
        printf("Error de memoria");
        return;
    }

    if (mercado->headProveedores==NULL)
    {
        printf("No hay proveedores para comprar.\n\n");
        return;
    }
    else
    {
        mostrarProveedores(mercado->headProveedores);
        printf("\nIngrese el ID del proveedor al que se le comprara: ");
        scanf("%d", &prov);
        while (recP != NULL)
        {
            if (recP->datosProveedores->id == prov)
            {
                proveedorEncontrado = recP->datosProveedores;
                break;
            }
            recP = recP->sig;
        }
        if (proveedorEncontrado == NULL)
        {
            printf("El proveedor no se encuentra registrado.\n\n");
            return;
        }
        convertirAMayusculas(proveedorEncontrado->nombre);
        printf("NOMBRE PROVEEDOR : %s\n",proveedorEncontrado->nombre);
    }

    printf("\nIngrese el ID del producto: ");
    scanf("%d", &(nuevoProducto->id));

    if (buscarProducto(mercado->productos->abbProducto, nuevoProducto->id) != NULL)
    {
        agregarCant(mercado->productos->abbProducto,nuevoProducto->id);
        return;
    }

    datosProducto(nuevoProducto);

    if(nuevoProducto->stock == 0)
    {
        printf("\n## PRODUCTOS CON STOCK 0 NO PUEDEN SER AGREGADOS ##\n\n");
        return;
    }

    mercado->productos->abbProducto = insertarProductos(mercado->productos->abbProducto, nuevoProducto);

    printf("\nProducto agregado con exito\n\n");
    return;
}

struct NodoProductos* encontrarNodoMinimo(struct NodoProductos* nodo)
{
    struct NodoProductos* actual = nodo;
    while (actual->izq != NULL)
    {
        actual = actual->izq;
    }
    return actual;
}

struct NodoProductos* eliminarNodoABB(struct NodoProductos* nodo, int idProducto)
{
    if (nodo == NULL)
    {
        return nodo;
    }

    if (idProducto < nodo->datosProducto->id)
    {
        nodo->izq = eliminarNodoABB(nodo->izq, idProducto);
    }
    else if (idProducto > nodo->datosProducto->id)
    {
        nodo->der = eliminarNodoABB(nodo->der, idProducto);
    }
    else
    {
        // Nodo sin hijos o con un solo hijo
        if (nodo->izq == NULL)
        {
            struct NodoProductos* temp = nodo->der;
            return temp;
        }
        else if (nodo->der == NULL)
        {
            struct NodoProductos* temp = nodo->izq;
            return temp;
        }

        // Nodo con dos hijos
        struct NodoProductos* temp = encontrarNodoMinimo(nodo->der);
        nodo->datosProducto->id = temp->datosProducto->id;
        nodo->datosProducto->nombre = temp->datosProducto->nombre;
        nodo->datosProducto->precio = temp->datosProducto->precio;
        nodo->der = eliminarNodoABB(nodo->der, temp->datosProducto->id);
    }
    return nodo;
}

void eliminarProducto(struct MercaditoLibre* mercado)
{
    int idProducto;

    if(mercado->productos->abbProducto == NULL)
    {
        printf("No hay productos registrados para eliminar.\n\n");
        return;
    }
    else
    {
        printf("\nIngrese el id del producto que desea eliminar: ");
        scanf("%d", &idProducto);
        mercado->productos->abbProducto = eliminarNodoABB(mercado->productos->abbProducto, idProducto);
        printf("Producto con ID %d eliminado correctamente.\n\n", idProducto);
        return;
    }

}

void mostrarInforme(struct MercaditoLibre *mercado)
{
    struct NodoClientes *rec = mercado->headClientes;

    if (rec == NULL)
    {
        printf("## NO ES POSIBLE MOSTRAR EL INFORME DE VENTAS DEBIDO A QUE LA LISTA DE CLIENTES ESTA VACIA ##\n\n");
        return;
    }

    struct Cliente *clienteMayor = rec->datosCliente;
    while (rec != NULL)
    {
        if (rec->datosCliente->gastado > clienteMayor->gastado)
        {
            clienteMayor = rec->datosCliente;
        }
        rec = rec->sig;
    }

    if (clienteMayor->gastado==0)
    {
        printf("## NO ES POSIBLE MOSTRAR UN INFORME DE VENTAS DEBIDO A QUE NO SE HAN REALIZADO COMPRAS ##\n\n");
        return;
    }
    printf("## CLIENTE QUE MAS HA GASTADO ##\n");
    printf(" -------------------------------\n");
    printf("|%-*s|%-*s|\n",+ 15, "Nombre",+ 15, "ID");
    printf(" -------------------------------\n");
    printf("|%-*s|%-*d|\n", -15, clienteMayor->nombre, -15, clienteMayor->id);
    printf(" -------------------------------\n");
    printf(" --------------------------------------------------\n");
    printf("| MONTO TOTAL GASTADO DEL CLIENTE: %-15d |\n",clienteMayor->gastado);
    printf(" --------------------------------------------------\n\n");
    printf("## GANANCIAS DE LA TIENDA ##\n");
    printf(" ----------------------------------\n");
    printf("| GANANCIAS TOTALES:%-10d     |\n",calcularTotalGastado(mercado));
    printf(" ----------------------------------\n\n");
    return;
}

void mostrarRotacion(struct MercaditoLibre *mercado)
{
    struct Productos *productoConMayorRotacion = obtenerProductoMayorRotacion(mercado->productos->abbProducto);
    struct Productos *productoConMenorRotacion = obtenerProductoMenorRotacion(mercado->productos->abbProducto);

    if (productoConMayorRotacion != NULL)
    {
        printf("## ROTACION DE PRODUCTOS ##\n");
        printf(" --------------------------------------------------\n");
        printf("| Producto con mayor rotacion: %-10s          |\n", productoConMayorRotacion->nombre);
        printf(" --------------------------------------------------\n");
        printf(" --------------------------------------------------\n");
        printf("| Producto con menor rotacion: %-10s          |\n", productoConMenorRotacion->nombre);
        printf(" --------------------------------------------------\n\n");
    }
    else
    {
        printf("## NO ES POSIBLE MOSTRAR LA ROTACION DE PRODUCTOS DEBIDO A QUE NO HAY PRODUCTOS REGISTRADOS ##\n\n");
    }

}

//editar main para que ofresca opciones y dsps elegir si es cliente proveedor etc//

int main()
{
    struct MercaditoLibre *mercado = (struct MercaditoLibre *)malloc(sizeof(struct MercaditoLibre));
    mercado->headClientes = NULL;
    mercado->headProveedores = NULL;
    mercado->productos = (struct listaProductos *)malloc(sizeof(struct listaProductos));
    mercado->productos->abbProducto = NULL;

    int opcion1, opcion2,opcion3;
    int id, precio;
    char nombre[50];

    do
    {
        printf("#####################################\n");
        printf("## Hola, elige lo que deseas hacer ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 1: Agregar               ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 2: Quitar                ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 3: Modificar             ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 4: Mostrar               ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 5: Vender                ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 6: Informe y rotacion    ##\n");
        printf("## ------------------------------- ##\n");
        printf("## Opcion 0: Salir                 ##\n");
        printf("#####################################\n\n");
        printf("Ingrese su respuesta: ");
        scanf("%d", &opcion1);

        switch (opcion1)
        {
            case 1:
            {
                //system("cls");
                printf(" ------------------------ \n");
                printf("|Que desea agregar       |\n");
                printf("|------------------------|\n");
                printf("|1 Clientes              |\n");
                printf("|------------------------|\n");
                printf("|2 Proveedores           |\n");
                printf("|------------------------|\n");
                printf("|3 Comprar Productos     |\n");
                printf("|------------------------|\n");
                printf("|4 Volver                |\n");
                printf(" ------------------------ \n");
                printf("Ingrese su respuesta: ");
                scanf("%d", &opcion2);
                printf("\n");

                if (opcion2 == 1)
                {
                    //system("cls");
                    agregarCliente(mercado,&(mercado->headClientes));
                }
                else if (opcion2 == 2)
                {
                    //system("cls");
                    agregarProveedor(mercado,&(mercado->headProveedores));
                }
                else if (opcion2 == 3)
                {
                    //system("cls");
                    agregarProducto(mercado);
                }
                else
                {
                    //system("cls");
                }
                break;
            }

            case 2:
            {
                //system("cls");
                printf(" ------------------------ \n");
                printf("|Que desea quitar        |\n");
                printf("|------------------------|\n");
                printf("|1 Clientes              |\n");
                printf("|------------------------|\n");
                printf("|2 Proveedores           |\n");
                printf("|------------------------|\n");
                printf("|3 Productos             |\n");
                printf("|------------------------|\n");
                printf("|4 Volver                |\n");
                printf(" ------------------------ \n");
                printf("Ingrese su respuesta: ");
                scanf("%d", &opcion2);
                printf("\n");

                if (opcion2 == 1)
                {
                    //system("cls");
                    eliminarCliente(mercado);
                }
                else if (opcion2 == 2)
                {
                    //system("cls");
                    eliminarProveedor(mercado);
                }
                else if (opcion2 == 3)
                {
                    //system("cls");
                    eliminarProducto(mercado);
                }
                else
                {
                    //system("cls");
                }
                break;
            }

            case 3:
            {
                //system("cls");
                printf(" ------------------------ \n");
                printf("|Que desea modificar     |\n");
                printf("|------------------------|\n");
                printf("|1 Clientes              |\n");
                printf("|------------------------|\n");
                printf("|2 Proveedores           |\n");
                printf("|------------------------|\n");
                printf("|3 Productos             |\n");
                printf("|------------------------|\n");
                printf("|4 Volver                |\n");
                printf(" ------------------------ \n");
                printf("Ingrese su respuesta: ");
                scanf("%d", &opcion2);
                printf("\n");

                if (opcion2 == 1)
                {
                    //system("cls");
                    modificarCliente(mercado);
                }
                else if (opcion2 == 2)
                {
                    //system("cls");
                    modificarProveedor(mercado);
                }
                else if (opcion2 == 3)
                {
                    //system("cls");
                    modificarProductoabb(mercado);
                }
                else
                {
                    //system("cls");
                }
                break;
            }

            case 4:
            {
                //system("cls");
                printf(" ------------------------ \n");
                printf("|Que desea ver           |\n");
                printf("|------------------------|\n");
                printf("|1 Clientes              |\n");
                printf("|------------------------|\n");
                printf("|2 Proveedores           |\n");
                printf("|------------------------|\n");
                printf("|3 Productos             |\n");
                printf("|------------------------|\n");
                printf("|4 Volver                |\n");
                printf(" ------------------------ \n");
                printf("Ingrese su respuesta: ");
                scanf("%d", &opcion2);
                printf("\n");

                if (opcion2 == 1)
                {
                    //system("cls");
                    printf(" ------------------------ \n");
                    printf("|Que desea ver           |\n");
                    printf("|------------------------|\n");
                    printf("|1 Lista de clientes     |\n");
                    printf("|------------------------|\n");
                    printf("|2 Compras de un cliente |\n");
                    printf(" ------------------------ \n");
                    printf("Ingrese su respuesta: ");
                    scanf("%d", &opcion3);
                    printf("\n");
                    if(opcion3 == 1)
                    {
                        //system("cls");
                        mostrarClientes(mercado->headClientes);
                    }
                    else if (opcion3 == 2)
                    {
                        //system("cls");
                        mostrarComprasClientes(mercado);
                    }
                }
                else if (opcion2 == 2)
                {
                    //system("cls");
                    mostrarProveedores(mercado->headProveedores);
                }
                else if (opcion2 == 3)
                {
                    //system("cls");
                    mostrarProductos(mercado->productos->abbProducto);
                }
                else
                {
                    //system("cls");
                }
                break;
            }

            case 5:
            {
                //system("cls");
                venderProducto(mercado);
                break;
            }

            case 6:
            {
                //system("cls");
                mostrarInforme(mercado);
                mostrarRotacion(mercado);
                break;
            }

            case 0:
            {
                //system("cls");
                printf("## GRACIAS POR PREFERIRNOS ##");
                break;
            }


            default:
            {
                //system("cls");
                printf("\nOpcion no valida. Por favor, elija una opcion valida.\n\n");
                break;
            }
        }

    } while (opcion1 != 0);

    return 0;
}