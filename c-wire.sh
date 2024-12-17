#!/bin/bash

# Usage: ./c-wire.sh <chemin_csv> <type_station> <type_consommateur> [id_centrale]

if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <chemin_csv> <type_station> <type_consommateur> [id_centrale]"
    echo "Options :"
    echo "  <type_station> : hvb, hva, lv"
    echo "  <type_consommateur> : comp, indiv, all"
    exit 1
fi

CSV_PATH=$1
TYPE_STATION=$2
TYPE_CONSUMER=$3
CENTRALE_ID=$4
OUTPUT_DIR="output"
TMP_DIR="tmp"
GRAPHS_DIR="graphs"

if [[ "$TYPE_STATION" != "hvb" && "$TYPE_STATION" != "hva" && "$TYPE_STATION" != "lv" ]]; then
    echo "Erreur : type de station invalide. Choisir parmi : hvb, hva, lv."
    exit 1
fi

if [[ "$TYPE_CONSUMER" != "comp" && "$TYPE_CONSUMER" != "indiv" && "$TYPE_CONSUMER" != "all" ]]; then
    echo "Erreur : type de consommateur invalide. Choisir parmi : comp, indiv, all."
    exit 1
fi

if [[ ("$TYPE_STATION" == "hvb" || "$TYPE_STATION" == "hva") && "$TYPE_CONSUMER" != "comp" ]]; then
    echo "Erreur : seuls les consommateurs de type 'comp' sont autorisés pour hvb et hva."
    exit 1
fi

mkdir -p "$OUTPUT_DIR" "$TMP_DIR" "$GRAPHS_DIR"


if [ ! -f "$CSV_PATH" ]; then
    echo "Erreur : le fichier $CSV_PATH n'existe pas."
    exit 1
fi

if [ ! -f "./cwire" ]; then
    echo "Compilation du programme C..."
    make;
    if [ "$?" -ne 0 ]; then
        echo "Erreur : Échec de la compilation du programme C."
        exit 1
    fi
fi

FILTERED_CSV="$TMP_DIR/filtered.csv"
if [ -n "$CENTRALE_ID" ]; then
    awk -F";" -v station="$TYPE_STATION" -v central="$CENTRALE_ID" '$1 == central && $2 == station' "$CSV_PATH" > "$FILTERED_CSV"
else
    awk -F";" -v station="$TYPE_STATION" '$2 == station' "$CSV_PATH" > "$FILTERED_CSV"
fi

START_TIME=$(date +%s)

./code/cwire "$FILTERED_CSV" "$TYPE_STATION" "$TYPE_CONSUMER" > "$OUTPUT_DIR/result_${TYPE_STATION}_${TYPE_CONSUMER}.csv"
if [ "$?" -ne 0 ]; then
    echo "Erreur : échec du traitement des données par le programme C."
    exit 1
fi

END_TIME=$(date +%s)
DURATION=$((END_TIME - START_TIME))
echo "Traitement terminé en $DURATION secondes. Résultats dans $OUTPUT_DIR."
