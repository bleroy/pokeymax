data = csvread('envelope.csv');

shapes = data(:,3);
shapes = unique(shapes);

figure;
for i=1:numel(shapes)
	shape = shapes(i);
	idx = ismember(data(:,3),shape);
	shapedata = data(idx,:);
	shapedata = shapedata(1:10000,:);
	shapedata(:,1) = shapedata(:,1)-shapedata(1,1);
	subplot(4,4,shape+1);
	plot(shapedata(:,1),shapedata(:,2));
end

