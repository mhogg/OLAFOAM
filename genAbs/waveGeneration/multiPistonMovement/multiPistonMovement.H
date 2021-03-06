/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | Copyright (C) 2012 OpenFOAM Foundation
     \\/     M anipulation  |
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Description
    fixedValue pointPatchField.

    - without state

SourceFiles
    multiPistonMovement.C

\*---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------*\
| olaFoam Project                                       ll                    |
|                                                       l l                   |
|   Coder: Pablo Higuera Caubilla                 ooo   l l     aa            |
|   Bug reports: phicau@gmail.com                o   o  l l    a  a           |
|                                                o   o  ll   l a  aa  aa      |
|                                                 ooo    llll   aa  aa        |
|                                                                             |
|                                                FFFFF OOOOO AAAAA M   M      |
|                                                F     O   O A   A MM MM      |
|  Formerly IHFOAM Project                       FFFF  O   O AAAAA M M M      |
|  Work initially developed at IH Cantabria      F     O   O A   A M   M      |
|                                                F     OOOOO A   A M   M      |
|   -----------------------------------------------------------------------   |
| References:                                                                 |
|                                                                             |
| - Realistic wave generation and active wave absorption for Navier-Stokes    |
|    models: Application to OpenFOAM.                                         |
|    Higuera, P., Lara, J.L. and Losada, I.J. (2013)                          |
|    Coastal Engineering, Vol. 71, 102-118.                                   |
|    http://dx.doi.org/10.1016/j.coastaleng.2012.07.002                       |
|                                                                             |
| - Simulating coastal engineering processes with OpenFOAM                    |
|    Higuera, P., Lara, J.L. and Losada, I.J. (2013)                          |
|    Coastal Engineering, Vol. 71, 119-134.                                   |
|    http://dx.doi.org/10.1016/j.coastaleng.2012.06.002                       |
|                                                                             |
| - Three-dimensional numerical wave generation with moving boundaries        |
|    Higuera, P., Losada, I.J. and Lara, J.L. (2015)                          |
|    Coastal Engineering, Vol. 101, 35-47.                                    |
|    http://dx.doi.org/10.1016/j.coastaleng.2015.04.003                       |
|                                                                             |
\*---------------------------------------------------------------------------*/

#ifndef fixedValuePointPatchTemplateVectorField_H
#define fixedValuePointPatchTemplateVectorField_H

#include "fixedValuePointPatchFields.H"

#include "mathematicalConstants.H"
#include "fixedValueFvPatchFields.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{

class multiPistonMovement
:
    #if OFFLAVOUR == 1
        public FixedValuePointPatchField
        <
            pointPatchField,
            pointMesh,
            pointPatch,
            DummyMatrix,
            vector
        >
    #else
        public fixedValuePointPatchField<vector>
    #endif
{
    // Private data

        //- Dictionary name
        word multiPistonDictName_;

        //- Time series for interpolation
        scalarList timeSeries_;

        //- Number of different paddles (for absorption)
        label nPaddles_;

        //- Paddle position series for interpolation
        List<List<scalar> > paddlePosition_;

        //- Free surface series at the paddle for interpolation
        List<List<scalar> > paddleEta_;

        //- Initial water depth (meters)
        scalar initialWaterDepth_;

        //- Perpendicular direction to the paddle
        vector meanAngle_;

        //- Generation + Absorption at the same time
        bool genAbs_;

        //- Drift Prevention System acting
        List<bool> DPS_;

        //- Maximum allowed stroke
        scalar maxStroke_;

        //- DPS period
        scalar DPST_;

        //- Drift Prevention System sign (+/-1)
        scalarList DPSsign_;

        //- DPS starting time
        scalarList DPStIni_;

        //- Instantaneous paddle position correction 
        scalarList instDPSCorrection_;

        //- Cumulative paddle position correction 
        scalarList cumDPSCorrection_;

        //- Cumulative paddle position correction
        scalarList cumAbsCorrection_;

        //- Fade in time (linear 0-1)
        scalar tSmooth_;

        //- Tuning factor
        scalar tuningFactor_;

        //- BC has been checked for first time
        bool allCheck_;

public:

    //- Runtime type information
    TypeName("multiPistonMovement");


    // Constructors

        //- Construct from patch and internal field
        multiPistonMovement
        (
            const pointPatch&,
            const DimensionedField<vector, pointMesh>&
        );

        //- Construct from patch, internal field and dictionary
        multiPistonMovement
        (
            const pointPatch&,
            const DimensionedField<vector, pointMesh>&,
            const dictionary&,
            const bool valueRequired=true
        );

        //- Construct by mapping a copy onto a new patch
        multiPistonMovement
        (
            const multiPistonMovement&,
            const pointPatch&,
            const DimensionedField<vector, pointMesh>&,
            #if OFFLAVOUR == 1
                const PointPatchFieldMapper&
            #else
                const pointPatchFieldMapper&
            #endif
        );

        //- Construct as copy
        multiPistonMovement
        (
            const multiPistonMovement&
        );

        //- Construct and return a clone
        virtual autoPtr< pointPatchField<vector> > clone() const
        {
            return autoPtr< pointPatchField<vector> >
            (
                new multiPistonMovement(*this)
            );
        }

        //- Construct as copy setting internal field reference
        multiPistonMovement
        (
            const multiPistonMovement&,
            const DimensionedField<vector, pointMesh>&
        );

        //- Construct and return a clone setting internal field reference
        virtual autoPtr< pointPatchField<vector> > clone
        (
            const DimensionedField<vector, pointMesh>& iF
        ) const
        {
            return autoPtr< pointPatchField<vector> >
            (
                new multiPistonMovement(*this, iF)
            );
        }


    //- Destructor
    virtual ~multiPistonMovement();


    // Member functions

        //- Update the coefficients associated with the patch field
        virtual void updateCoeffs();

        //- Write
        virtual void write(Ostream&) const;

        #include "pistonMemberFun.H"

};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //

