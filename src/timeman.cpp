/*
  Stockfish, a UCI chess playing engine derived from Glaurung 2.1
  Copyright (C) 2004-2026 The Stockfish developers (see AUTHORS file)

  Stockfish is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Stockfish is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "timeman.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>

#include "search.h"
#include "ucioption.h"

namespace Stockfish {

TimePoint TimeManagement::optimum() const { return optimumTime; }
TimePoint TimeManagement::maximum() const { return maximumTime; }

void TimeManagement::clear() {
    availableNodes = -1;  // When in 'nodes as time' mode
}

void TimeManagement::advance_nodes_time(std::int64_t nodes) {
    assert(useNodesTime);
    availableNodes = std::max(int64_t(0), availableNodes - nodes);
}

// Called at the beginning of the search and calculates
// the bounds of time allowed for the current game ply.
void TimeManagement::init(Search::LimitsType& limits,
                          Color                us,
                          int                  ply,
                          const OptionsMap&    options,
                          double&              originalTimeAdjust) {
    TimePoint npmsec = TimePoint(options["nodestime"]);

    startTime    = limits.startTime;
    useNodesTime = npmsec != 0;

    if (limits.time[us] == 0)
        return;

    TimePoint moveOverhead = TimePoint(options["Move Overhead"]);

    if (useNodesTime)
    {
        if (availableNodes == -1)
            availableNodes = npmsec * limits.time[us];

        limits.time[us] = TimePoint(availableNodes);
        limits.inc[us] *= npmsec;
        limits.npmsec = npmsec;
        moveOverhead *= npmsec;
    }

    // --- PROTOCOLO ALETHEIA 99% ---
    // Calculamos el tiempo neto disponible restando el margen de seguridad
    TimePoint available = std::max(TimePoint(1), limits.time[us] - moveOverhead);

    // Inyección de agresividad absoluta: 99% del tiempo disponible
    optimumTime = TimePoint(available * 0.99);
    maximumTime = TimePoint(available * 0.99);

    // Trigger de Resistencia FS: Si nuestro tiempo es < 30% que el del enemigo
    if (limits.time[us] < limits.time[!us] * 0.30)
    {
        optimumTime = TimePoint(available * 0.40);
        maximumTime = TimePoint(available * 0.40);
    }

    // --- MODIFICACIÓN HÍBRIDA ALETHEIA (PROPUESTAS 1, 2, 3) ---
    // Implementación para corregir la ruptura en finales (Movida 55+)
    
    // Propuesta 1: Umbral de activación por movida o material reducido
    // 1500 unidades de material equivale aproximadamente a dos piezas menores por bando.
    if (ply / 2 > 55)
    {
        // Propuesta 2: Piso de seguridad basado en el incremento (Inversión obligatoria)
        // Forzamos al motor a usar al menos el 80% del incremento (6s -> 4.8s)
        TimePoint minAllowed = TimePoint(limits.inc[us] * 0.8);
        
        if (optimumTime < minAllowed)
            optimumTime = minAllowed;

        // Propuesta 3: Gestión de inestabilidad (Freno de mano táctico)
        // Nota: bestValue y lastValue deben ser monitoreados en el bucle de búsqueda.
        // Aquí ajustamos el máximo permitido para dar margen a extensiones si el score oscila.
        maximumTime = std::max(maximumTime, TimePoint(optimumTime * 1.5));
    }
    // --- FIN DE LA MODIFICACIÓN HÍBRIDA ---

    if (options["Ponder"])
        optimumTime += optimumTime / 4;
    // --- FIN DEL PROTOCOLO ---
}

}  // namespace Stockfish
