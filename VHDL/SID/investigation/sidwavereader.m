## Copyright (C) 2019 Mark
## 
## This program is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 3 of the License, or
## (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program.  If not, see <http://www.gnu.org/licenses/>.

## -*- texinfo -*- 
## @deftypefn {} {@var{retval} =} sidwavereader (@var{input1}, @var{input2})
##
## @seealso{}
## @end deftypefn

## Author: Mark <markw@Eraze>
## Created: 2019-11-16

function [waves,bad] = sidwavereader ()

  path = 'sidwaves';
  
  waves = zeros(4096,30,16);
  for i=1:15
    wavepath = fullfile(path,upper(sprintf('WAVE%01x.csv',i)));
    wave = csvread(wavepath);
    waves(:,:,i+1) = wave*16;
  end
%  waves(:,end+1,:) = waves(:,22,:);
%  waves(1:2048,end,2) = 2*(1:2048)-1;
%  waves(4096:-1:2049,end,2) = 2*(1:2048)-1;
%  waves(:,end,2) = bitand(waves(:,end,2),4096-1-1);
  
%  waves(1:4096,end,3) = (1:4096)-1;
%  waves(1:4096,end,5) = 4095;
  
%  badf = [1 3 11 12 24 25 26 27 28 29 30];
%  bad = false(31,1);
%  bad(badf) = true; 
endfunction
