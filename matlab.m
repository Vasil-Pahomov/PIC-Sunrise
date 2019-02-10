% моделирование нелинейного изменения фазы для определения времени разгорания лампы
clear;clc;
t=1;
period=1050;
increase_divider=400;
min_divider=50;


divCounter=increase_divider;
incDivider=increase_divider;
phase=period;
while (true)
    
  if (rem(t,100)==1)
    ph(ceil(t/100))=phase;
    div(ceil(t/100))=incDivider;
  end
  t=t+1;
  
  divCounter=divCounter-1;
  if (divCounter <= 0) 
    if (phase > 1) 
	    phase=phase-1;
    else
        break;
    end 
    divCounter=incDivider;
    if (incDivider > min_divider) 
	   incDivider=incDivider-1;
    end
  end
end

figure(1);plot((1:length(ph))/60,ph);
figure(2);plot((1:length(div))/60,div);
